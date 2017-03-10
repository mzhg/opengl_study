#pragma once
#include <vector>
#include <memory>
#include <map>

#include "Texture.h"
#include "MaxRectsBinPack.h"

namespace jet
{
	namespace util
	{
	
		typedef struct PackSettings
		{
			bool pot;
			int  paddingX, paddingY;
			bool edgePadding;
			bool duplicatePadding;
			bool rotation;
			int minWidth, minHeight;
			int maxWidth, maxHeight;
			bool square;
			bool stripWhitespaceX, stripWhitespaceY;
			int alphaThreshold;
			int format; // GL Format
			bool alias;
			bool outputArray;
			bool fast;
			bool arrayed;

			PackSettings() :
				pot(true),
				paddingX(2),
				paddingY(2),
				edgePadding(true),
				duplicatePadding(false),
				rotation(false),
				minWidth(128),
				minHeight(128),
				maxWidth(4096),
				maxHeight(4096),
				square(false),
				stripWhitespaceX(false),
				stripWhitespaceY(false),
				alphaThreshold(0),
				format(GL_RGBA8),
				alias(true),
				outputArray(false),
				fast(false),
				arrayed(false)
				{}
		}PackSettings;

		class TexturePacker
		{
		public:
			void pack(const class TextureAssembly& input, const PackSettings& settings, class TextureAtlas* output);
		
		private:
			RectPagePtr packPage(std::vector<RectSize>& inputRects);
			RectPagePtr packAtSize(bool fully, int width, int height, std::vector<RectSize>& inputRects);

			void generateTextureAtlas(class TextureAtlas* output);
			const Texture2D* getTexture(int id) const;

		private:
			std::map<std::string, int> m_namesToIDS;
			std::map<int, std::string> m_idsToNames;
			std::vector<RectPagePtr> m_pages;

			MaxRectsBinPack m_maxRects;
			PackSettings m_settings;
			const TextureAssembly* m_TextureAssembly;
		};

		class TextureAssembly
		{
		public:
			void add(const std::string& name, Texture2D* pTex)
			{
				std::pair<std::string, Texture2D*> value(name, pTex);
				m_textures.insert(value);
			}

			const Texture2D* get(const std::string& name) const 
			{ 
				const auto it = m_textures.find(name);
				return it == m_textures.end() ? nullptr : it->second;
			}

			const std::map<std::string, Texture2D*>& getValues() const{ return m_textures; }
		private:
			friend class TexturePacker;
			std::map<std::string, Texture2D*> m_textures;
		};

		typedef struct TextureRegion
		{
			Rectangle2i Region;
			Texture2D* pTexture;
			int Layer;           // Must be 0 if texture is not a array.
			bool Rotated;

			TextureRegion() :Region(), pTexture(nullptr), Layer(0), Rotated(false){}


			glm::mat4 getTexCoordTransform() const;
		}TextureRegion;

		class TextureAtlas
		{
		public:
			Texture2D* getTexture(uint32_t index = 0);
			const TextureRegion& getTextureRegion(const std::string& string);

			virtual ~TextureAtlas();
		private:
			friend class TexturePacker;
			std::map<std::string, TextureRegion> m_textureRegions;
			std::vector<Texture2D*>  m_textures;
		};
	}
}



