#include "TexturePacker.h"
#include "geometry2d.h"
#include "MaxRectsBinPack.h"
#include <algorithm>

#include <glm.hpp>
#include <gtc\quaternion.hpp>
#include <gtc\matrix_transform.hpp>

namespace jet
{
	namespace util
	{
		static TextureRegion g_Blank;

		glm::mat4 TextureRegion::getTexCoordTransform() const
		{
			assert(pTexture);

			const float width_scale = Numeric::ratio(Region.Width, pTexture->getWidth());
			const float height_scale = Numeric::ratio(Region.Height, pTexture->getHeight());
			const float x = Numeric::ratio(Region.X, pTexture->getWidth());
			const float y = Numeric::ratio(Region.X, pTexture->getHeight());

			if (!Rotated)
			{
				glm::mat4 mat = glm::mat4(1.0f);
				mat[0][0] = width_scale;
				mat[1][1] = height_scale;
				mat[3][0] = x;
				mat[3][1] = y;

				return mat;
			}
			else
			{
				glm::mat4 mat = glm::mat4(1.0f);
				mat[0][0] = height_scale;
				mat[1][1] = width_scale;
				mat[3][0] = x;
				mat[3][1] = y;

				glm::mat4 trans_to_local = glm::mat4(1.0f);
				trans_to_local[3] = {-x, -y, 0.0f, 1.0f};

				glm::mat4 trans_to_world = glm::mat4(1.0f);
				const float angle90 = glm::radians(90.0f);
				const glm::vec3 zAxis(0, 0, 1);
				glm::mat4 rotate90 = glm::rotate(trans_to_world, angle90, zAxis);

				trans_to_world[3] = { width_scale + x, y, 0.0f, 1.0f };

				return trans_to_world * rotate90 * trans_to_local * mat;
			}
		}

		TextureAtlas::~TextureAtlas()
		{
			for (auto it : m_textures)
			{
				delete it;
			}

			m_textures.clear();
		}

		Texture2D* TextureAtlas::getTexture(uint32_t index)
		{
			if (index >= m_textures.size())
			{
				return nullptr;
			}
			return m_textures[index];
		}
		const TextureRegion& TextureAtlas::getTextureRegion(const std::string& string)
		{
			const auto it = m_textureRegions.find(string);
			if (it == m_textureRegions.end())
			{
				return g_Blank;
			}
			else
			{
				return it->second;
			}
		}
		typedef bool boolean;

		/** Returns the next power of two. Returns the specified value if the value is already a power of two. */
		static int nextPowerOfTwo(int value) 
		{
			if (value == 0) return 1;
			value--;
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			return value + 1;
		}

		class BinarySearch
		{
		public:
			int min, max, fuzziness, low, high, current;
			bool pot;

			BinarySearch(int min, int max, int fuzziness, boolean pot) {
				this->pot = pot;
				this->fuzziness = pot ? 0 : fuzziness;
				this->min = pot ? (int)(log(nextPowerOfTwo(min)) / log(2.0)) : min;
				this->max = pot ? (int)(log(nextPowerOfTwo(max)) / log(2.0)) : max;
			}

			int reset() {
				low = min;
				high = max;
				current = (low + high) >> 1;
				return pot ? pow(2, current) : current;
			}

			int next(boolean result) {
				if (low >= high) return -1;
				if (result)
					low = current + 1;
				else
					high = current - 1;
				current = (low + high) >> 1;
				if (abs(low - high) < fuzziness) return -1;
				return pot ? pow(2, current) : current;
			}
		};

		static void checkSettings(const PackSettings& settings)
		{
			assert(settings.minWidth <= settings.maxWidth);
			assert(settings.minHeight <= settings.maxHeight);
		}

		void TexturePacker::pack(const TextureAssembly& input, const PackSettings& settings, TextureAtlas* output)
		{
			if (input.m_textures.empty())
			{
				return;
			}

			checkSettings(settings);
			m_settings = settings;
			m_TextureAssembly = &input;

			std::vector<RectSize> inputRects;
			int currentUniqueID = 0;
			
			inputRects.resize(input.m_textures.size());
			for (const auto it : input.getValues())
			{
				RectSize size = { it.second->getWidth(), it.second->getHeight(), currentUniqueID };
				std::pair<std::string, int> value(it.first, currentUniqueID);
				m_namesToIDS.insert(value);
				m_idsToNames.insert(std::pair<int, std::string>(value.second, value.first));
				inputRects[currentUniqueID] = size;

				currentUniqueID++;
			}
			
			if (settings.fast)
			{
				std::sort(inputRects.begin(), inputRects.end(), [](const RectSize& o1, const RectSize& o2)
				{
					int n1 = o1.Width > o1.Height ? o1.Width : o1.Height;
					int n2 = o2.Width > o2.Height ? o2.Width : o2.Height;
					return n1 < n2;
				});
			}

			m_pages.clear();
			while (inputRects.size() > 0)
			{
				RectPagePtr result = packPage(inputRects);
				m_pages.push_back(result);
				inputRects.swap(result->RemainingRects);
			}

			generateTextureAtlas(output);
		}

		static RectPagePtr getBest(RectPagePtr result1, RectPagePtr result2) 
		{
			if (result1.get() == nullptr) return result2;
			if (result2.get() == nullptr) return result1;

			return result1->Occupancy > result2->Occupancy ? result1 : result2;
		}

		const Texture2D* TexturePacker::getTexture(int id) const
		{
			assert(id != -1);
			const auto it = m_idsToNames.find(id);
			if (it != m_idsToNames.end())
			{
				return m_TextureAssembly->get(it->second);
			}
			else
			{
				return nullptr;
			}
		}

		RectPagePtr TexturePacker::packPage(std::vector<RectSize>& inputRects)
		{
			int paddingX = m_settings.paddingX, paddingY = m_settings.paddingY;
			float maxWidth = m_settings.maxWidth, maxHeight = m_settings.maxHeight;
			int edgePaddingX = 0, edgePaddingY = 0;
			if (m_settings.edgePadding) 
			{
				if (m_settings.duplicatePadding) 
				{ // If duplicatePadding, edges get only half padding.
					maxWidth -= paddingX;
					maxHeight -= paddingY;
				}
				else 
				{
					maxWidth -= paddingX * 2;
					maxHeight -= paddingY * 2;
					edgePaddingX = paddingX;
					edgePaddingY = paddingY;
				}
			}

			// Find min size.
			int minWidth = INT32_MAX, minHeight = INT32_MAX;
			for (size_t i = 0, nn = inputRects.size(); i < nn; i++) 
			{
				const RectSize& rect = inputRects[i];
				minWidth = Numeric::min(minWidth, rect.Width);
				minHeight = Numeric::min(minHeight, rect.Height);
				float width = rect.Width - paddingX, height = rect.Height - paddingY;
				if (m_settings.rotation) {
#if 0
					if ((width > maxWidth || height > maxHeight) && (width > maxHeight || height > maxWidth)) {
						String paddingMessage = (edgePaddingX > 0 || edgePaddingY > 0) ? (" and edge padding " + paddingX + "," + paddingY)
							: "";
						throw new RuntimeException("Image does not fit with max page size " + settings.maxWidth + "x" + settings.maxHeight
							+ paddingMessage + ": " + rect.name + "[" + width + "," + height + "]");
					}
#endif
					assert(!(width > maxWidth || height > maxHeight) && (width > maxHeight || height > maxWidth));
				}
				else {
#if 0
					if (width > maxWidth) {
						String paddingMessage = edgePaddingX > 0 ? (" and X edge padding " + paddingX) : "";
						throw new RuntimeException("Image does not fit with max page width " + settings.maxWidth + paddingMessage + ": "
							+ rect.name + "[" + width + "," + height + "]");
					}
					if (height > maxHeight && (!settings.rotation || width > maxHeight)) {
						String paddingMessage = edgePaddingY > 0 ? (" and Y edge padding " + paddingY) : "";
						throw new RuntimeException("Image does not fit in max page height " + settings.maxHeight + paddingMessage + ": "
							+ rect.name + "[" + width + "," + height + "]");
					}
#endif
					assert(width <= maxWidth);
					assert(!(height > maxHeight && (!m_settings.rotation || width > maxHeight)));
				}
			}
			minWidth = Numeric::max(minWidth, m_settings.minWidth);
			minHeight = Numeric::max(minHeight, m_settings.minHeight);

			printf("Packing\n");

			// Find the minimal page size that fits all rects.
			RectPagePtr bestResult;
			if (m_settings.square) 
			{
				int minSize = Numeric::max(minWidth, minHeight);
				int maxSize = Numeric::min(m_settings.maxWidth, m_settings.maxHeight);
				BinarySearch sizeSearch = BinarySearch(minSize, maxSize, m_settings.fast ? 25 : 15, m_settings.pot);
				int size = sizeSearch.reset(), i = 0;
				while (size != -1) 
				{
					RectPagePtr result = packAtSize(true, size - edgePaddingX, size - edgePaddingY, inputRects);
#if 0
					if (!m_settings.silent) {
						if (++i % 70 == 0) System.out.println();
						System.out.print(".");
					}
#endif
					bestResult = getBest(bestResult, result);
					size = sizeSearch.next(result.get() == nullptr);
				}
//				if (!m_settings.silent) System.out.println();
				// Rects don't fit on one page. Fill a whole page and return.
				if (bestResult.get() == nullptr) bestResult = packAtSize(false, maxSize - edgePaddingX, maxSize - edgePaddingY, inputRects);
//				sort.sort(bestResult.outputRects, rectComparator);
#if 0
				std::sort(bestResult->OutputRects.begin(), bestResult->OutputRects.end(), [](const Rect& a, const Rect& b)
				{
					return a.UniqueID < b.UniqueID;
				});
#endif
				bestResult->Width = Numeric::max(bestResult->Width, bestResult->Height);
				bestResult->Height = Numeric::max(bestResult->Width, bestResult->Height);
				return bestResult;
			}
			else 
			{
				BinarySearch widthSearch = BinarySearch(minWidth, m_settings.maxWidth, m_settings.fast ? 25 : 15, m_settings.pot);
				BinarySearch heightSearch = BinarySearch(minHeight, m_settings.maxHeight, m_settings.fast ? 25 : 15, m_settings.pot);
				int width = widthSearch.reset(), i = 0;
				int height = m_settings.square ? width : heightSearch.reset();
				while (true) {
					RectPagePtr bestWidthResult;
					while (width != -1) {
						RectPagePtr result = packAtSize(true, width - edgePaddingX, height - edgePaddingY, inputRects);
#if 0
						if (!settings.silent) {
							if (++i % 70 == 0) System.out.println();
							System.out.print(".");
						}
#endif
						bestWidthResult = getBest(bestWidthResult, result);
						width = widthSearch.next(result.get() == nullptr);
						if (m_settings.square) height = width;
					}
					bestResult = getBest(bestResult, bestWidthResult);
					if (m_settings.square) break;
					height = heightSearch.next(bestWidthResult.get() == nullptr);
					if (height == -1) break;
					width = widthSearch.reset();
				}
//				if (!m_settings.silent) System.out.println();
				// Rects don't fit on one page. Fill a whole page and return.
				if (bestResult.get() == nullptr)
					bestResult = packAtSize(false, m_settings.maxWidth - edgePaddingX, m_settings.maxHeight - edgePaddingY, inputRects);
//				sort.sort(bestResult.outputRects, rectComparator);
				return bestResult;
			}
		}

		static MaxRectsBinPack::FreeRectChoiceHeuristic ms_method[] =
		{
			MaxRectsBinPack::RectBestShortSideFit, ///< -BSSF: Positions the rectangle against the short side of a free rectangle into which it fits the best.
			MaxRectsBinPack::RectBestLongSideFit, ///< -BLSF: Positions the rectangle against the long side of a free rectangle into which it fits the best.
			MaxRectsBinPack::RectBestAreaFit, ///< -BAF: Positions the rectangle into the smallest free rect into which it fits.
			MaxRectsBinPack::RectBottomLeftRule, ///< -BL: Does the Tetris placement.
			MaxRectsBinPack::RectContactPointRule
		};

		RectPagePtr TexturePacker::packAtSize(bool fully, int width, int height, std::vector<RectSize>& inputRects)
		{
			RectPagePtr bestResult;
			for (int i = 0, n = _countof(ms_method); i < n; i++) 
			{
				m_maxRects.Init(width, height);
				RectPagePtr result;
				if (!m_settings.fast) 
				{
					result = m_maxRects.Pack(inputRects, ms_method[i]);
				}
				else 
				{
//					Array<Rect> remaining = new Array();
					std::vector<RectSize> remaining;
					for (size_t ii = 0, nn = inputRects.size(); ii < nn; ii++) 
					{
						const RectSize& rect = inputRects[ii];
						if (m_maxRects.Insert(rect.Width, rect.Height, rect.UniqueID, ms_method[i]).isValid() == false) 
						{
							while (ii < nn)
							{
//								remaining.add(inputRects.get(ii++));
								remaining.push_back(inputRects[ii++]);
							}
								
						}
					}
					result = m_maxRects.GetResult();
//					result.remainingRects = remaining;
					result->RemainingRects.swap(remaining);
				}
				if (fully && result->RemainingRects.size() > 0) continue;
				if (result->OutputRects.size() == 0) continue;
				bestResult = getBest(bestResult, result);
			}
			return bestResult;
		}
	}
}
