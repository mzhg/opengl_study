/** @file MaxRectsBinPack.cpp
	@author Jukka Jylänki

	@brief Implements different bin packer algorithms that use the MAXRECTS data structure.

	This work is released to Public Domain, do whatever you want with it.
*/
#include <utility>
#include <iostream>

#include <cassert>

#include "MaxRectsBinPack.h"
#include "Numeric.h"

namespace jet
{
	namespace util
	{
		using namespace std;

		MaxRectsBinPack::MaxRectsBinPack()
			:binWidth(0),
			binHeight(0)
		{}

		MaxRectsBinPack::MaxRectsBinPack(int width, int height)
		{
			Init(width, height);
		}

		void MaxRectsBinPack::Init(int width, int height)
		{
			binWidth = width;
			binHeight = height;

			Rect n;
			n.X = 0;
			n.Y = 0;
			n.Width = width;
			n.Height = height;

			usedRectangles.clear();

			freeRectangles.clear();
			freeRectangles.push_back(n);
		}

		RectPagePtr MaxRectsBinPack::GetResult()
		{
			int w = 0, h = 0;
			for (size_t i = 0; i < usedRectangles.size(); i++) 
			{
				const Rect& rect = usedRectangles[i];
				w = Numeric::max(w, rect.X + rect.Width);
				h = Numeric::max(h, rect.Y + rect.Height);
			}

			RectPagePtr result = RectPagePtr(new RectPage);
			result->OutputRects.insert(result->OutputRects.begin(), usedRectangles.begin(), usedRectangles.end());
			result->Occupancy = Occupancy();
			result->Width = w;
			result->Height = h;
			return result;
		}

		Rect MaxRectsBinPack::Insert(int width, int height, int uid, FreeRectChoiceHeuristic method)
		{
			Rect newNode;
			int score1; // Unused in this function. We don't need to know the score after finding the position.
			int score2;
			switch (method)
			{
			case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2); break;
			case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2); break;
			case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1); break;
			case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1); break;
			case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2); break;
			}

			if (newNode.Height == 0)
				return newNode;

			size_t numRectanglesToProcess = freeRectangles.size();
			for (size_t i = 0; i < numRectanglesToProcess; ++i)
			{
				if (SplitFreeNode(freeRectangles[i], newNode))
				{
					freeRectangles.erase(freeRectangles.begin() + i);
					--i;
					--numRectanglesToProcess;
				}
			}

			PruneFreeList();

			newNode.UniqueID = uid;
			newNode.Rotated = (newNode.Width != width);
			usedRectangles.push_back(newNode);
			return newNode;
		}

		RectPagePtr MaxRectsBinPack::Pack(std::vector<RectSize> &rects, FreeRectChoiceHeuristic method)
		{
			std::vector<RectSize> sources(rects.begin(), rects.end());
			std::vector<Rect> dst;
			Insert(sources, dst, method);

			RectPagePtr result = GetResult();
			result->RemainingRects.insert(result->RemainingRects.begin(), sources.begin(), sources.end());
			return result;
		}

		void MaxRectsBinPack::Insert(std::vector<RectSize> &rects, std::vector<Rect> &dst, FreeRectChoiceHeuristic method)
		{
			dst.clear();

			while (rects.size() > 0)
			{
				int bestScore1 = std::numeric_limits<int>::max();
				int bestScore2 = std::numeric_limits<int>::max();
				int bestRectIndex = -1;
				Rect bestNode;

				for (size_t i = 0; i < rects.size(); ++i)
				{
					int score1;
					int score2;
					Rect newNode = ScoreRect(rects[i].Width, rects[i].Height, method, score1, score2);

					if (score1 < bestScore1 || (score1 == bestScore1 && score2 < bestScore2))
					{
						bestScore1 = score1;
						bestScore2 = score2;
						bestNode = newNode;
						bestRectIndex = static_cast<int>(i);

						bestNode.Rotated = (newNode.Width != rects[i].Width);
						bestNode.UniqueID = rects[i].UniqueID;
					}
				}

				if (bestRectIndex == -1)
					return;

				PlaceRect(bestNode);
				rects.erase(rects.begin() + bestRectIndex);
			}
		}

		void MaxRectsBinPack::PlaceRect(const Rect &node)
		{
			size_t numRectanglesToProcess = freeRectangles.size();
			for (size_t i = 0; i < numRectanglesToProcess; ++i)
			{
				if (SplitFreeNode(freeRectangles[i], node))
				{
					freeRectangles.erase(freeRectangles.begin() + i);
					--i;
					--numRectanglesToProcess;
				}
			}

			PruneFreeList();

			usedRectangles.push_back(node);
			//		dst.push_back(bestNode); ///\todo Refactor so that this compiles.
		}

		Rect MaxRectsBinPack::ScoreRect(int width, int height, FreeRectChoiceHeuristic method, int &score1, int &score2) const
		{
			Rect newNode;
			score1 = std::numeric_limits<int>::max();
			score2 = std::numeric_limits<int>::max();
			switch (method)
			{
			case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2); break;
			case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2); break;
			case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1);
				score1 = -score1; // Reverse since we are minimizing, but for contact point score bigger is better.
				break;
			case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1); break;
			case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2); break;
			}

			// Cannot fit the current rectangle.
			if (newNode.Height == 0)
			{
				score1 = std::numeric_limits<int>::max();
				score2 = std::numeric_limits<int>::max();
			}

			return newNode;
		}

		/// Computes the ratio of used surface area.
		float MaxRectsBinPack::Occupancy() const
		{
			unsigned long usedSurfaceArea = 0;
			for (size_t i = 0; i < usedRectangles.size(); ++i)
				usedSurfaceArea += usedRectangles[i].Width * usedRectangles[i].Height;

			return (float)usedSurfaceArea / (binWidth * binHeight);
		}

		Rect MaxRectsBinPack::FindPositionForNewNodeBottomLeft(int width, int height, int &bestY, int &bestX) const
		{
			Rect bestNode;
			memset(&bestNode, 0, sizeof(Rect));

			bestY = std::numeric_limits<int>::max();

			for (size_t i = 0; i < freeRectangles.size(); ++i)
			{
				// Try to place the rectangle in upright (non-flipped) orientation.
				if (freeRectangles[i].Width >= width && freeRectangles[i].Height >= height)
				{
					int topSideY = freeRectangles[i].Y + height;
					if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].X < bestX))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = width;
						bestNode.Height = height;
						bestY = topSideY;
						bestX = freeRectangles[i].X;
					}
				}
				if (freeRectangles[i].Width >= height && freeRectangles[i].Height >= width)
				{
					int topSideY = freeRectangles[i].Y + width;
					if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].X < bestX))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = height;
						bestNode.Height = width;
						bestY = topSideY;
						bestX = freeRectangles[i].X;
					}
				}
			}
			return bestNode;
		}

		Rect MaxRectsBinPack::FindPositionForNewNodeBestShortSideFit(int width, int height,
			int &bestShortSideFit, int &bestLongSideFit) const
		{
			Rect bestNode;
//			memset(&bestNode, 0, sizeof(Rect));

			bestShortSideFit = std::numeric_limits<int>::max();

			for (size_t i = 0; i < freeRectangles.size(); ++i)
			{
				// Try to place the rectangle in upright (non-flipped) orientation.
				if (freeRectangles[i].Width >= width && freeRectangles[i].Height >= height)
				{
					int leftoverHoriz = abs(freeRectangles[i].Width - width);
					int leftoverVert = abs(freeRectangles[i].Height - height);
					int shortSideFit = Numeric::min(leftoverHoriz, leftoverVert);
					int longSideFit = Numeric::max(leftoverHoriz, leftoverVert);

					if (shortSideFit < bestShortSideFit || (shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = width;
						bestNode.Height = height;
						bestShortSideFit = shortSideFit;
						bestLongSideFit = longSideFit;
					}
				}

				if (freeRectangles[i].Width >= height && freeRectangles[i].Height >= width)
				{
					int flippedLeftoverHoriz = abs(freeRectangles[i].Width - height);
					int flippedLeftoverVert = abs(freeRectangles[i].Height - width);
					int flippedShortSideFit = Numeric::min(flippedLeftoverHoriz, flippedLeftoverVert);
					int flippedLongSideFit = Numeric::max(flippedLeftoverHoriz, flippedLeftoverVert);

					if (flippedShortSideFit < bestShortSideFit || (flippedShortSideFit == bestShortSideFit && flippedLongSideFit < bestLongSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = height;
						bestNode.Height = width;
						bestShortSideFit = flippedShortSideFit;
						bestLongSideFit = flippedLongSideFit;
					}
				}
			}
			return bestNode;
		}

		Rect MaxRectsBinPack::FindPositionForNewNodeBestLongSideFit(int width, int height,
			int &bestShortSideFit, int &bestLongSideFit) const
		{
			Rect bestNode;
//			memset(&bestNode, 0, sizeof(Rect));

			bestLongSideFit = std::numeric_limits<int>::max();

			for (size_t i = 0; i < freeRectangles.size(); ++i)
			{
				// Try to place the rectangle in upright (non-flipped) orientation.
				if (freeRectangles[i].Width >= width && freeRectangles[i].Height >= height)
				{
					int leftoverHoriz = abs(freeRectangles[i].Width - width);
					int leftoverVert = abs(freeRectangles[i].Height - height);
					int shortSideFit = Numeric::min(leftoverHoriz, leftoverVert);
					int longSideFit = Numeric::max(leftoverHoriz, leftoverVert);

					if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = width;
						bestNode.Height = height;
						bestShortSideFit = shortSideFit;
						bestLongSideFit = longSideFit;
					}
				}

				if (freeRectangles[i].Width >= height && freeRectangles[i].Height >= width)
				{
					int leftoverHoriz = abs(freeRectangles[i].Width - height);
					int leftoverVert = abs(freeRectangles[i].Height - width);
					int shortSideFit = Numeric::min(leftoverHoriz, leftoverVert);
					int longSideFit = Numeric::max(leftoverHoriz, leftoverVert);

					if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = height;
						bestNode.Height = width;
						bestShortSideFit = shortSideFit;
						bestLongSideFit = longSideFit;
					}
				}
			}
			return bestNode;
		}

		Rect MaxRectsBinPack::FindPositionForNewNodeBestAreaFit(int width, int height,
			int &bestAreaFit, int &bestShortSideFit) const
		{
			Rect bestNode = Rect();
//			memset(&bestNode, 0, sizeof(Rect));

			bestAreaFit = std::numeric_limits<int>::max();

			for (size_t i = 0; i < freeRectangles.size(); ++i)
			{
				int areaFit = freeRectangles[i].Width * freeRectangles[i].Height - width * height;

				// Try to place the rectangle in upright (non-flipped) orientation.
				if (freeRectangles[i].Width >= width && freeRectangles[i].Height >= height)
				{
					int leftoverHoriz = abs(freeRectangles[i].Width - width);
					int leftoverVert = abs(freeRectangles[i].Height - height);
					int shortSideFit = Numeric::min(leftoverHoriz, leftoverVert);

					if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = width;
						bestNode.Height = height;
						bestShortSideFit = shortSideFit;
						bestAreaFit = areaFit;
					}
				}

				if (freeRectangles[i].Width >= height && freeRectangles[i].Height >= width)
				{
					int leftoverHoriz = abs(freeRectangles[i].Width - height);
					int leftoverVert = abs(freeRectangles[i].Height - width);
					int shortSideFit = Numeric::min(leftoverHoriz, leftoverVert);

					if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = height;
						bestNode.Height = width;
						bestShortSideFit = shortSideFit;
						bestAreaFit = areaFit;
					}
				}
			}
			return bestNode;
		}

		/// Returns 0 if the two intervals i1 and i2 are disjoint, or the length of their overlap otherwise.
		static int CommonIntervalLength(int i1start, int i1end, int i2start, int i2end)
		{
			if (i1end < i2start || i2end < i1start)
				return 0;
			return Numeric::min(i1end, i2end) - Numeric::max(i1start, i2start);
		}

		int MaxRectsBinPack::ContactPointScoreNode(int x, int y, int width, int height) const
		{
			int score = 0;

			if (x == 0 || x + width == binWidth)
				score += height;
			if (y == 0 || y + height == binHeight)
				score += width;

			for (size_t i = 0; i < usedRectangles.size(); ++i)
			{
				if (usedRectangles[i].X == x + width || usedRectangles[i].X + usedRectangles[i].Width == x)
					score += CommonIntervalLength(usedRectangles[i].Y, usedRectangles[i].Y + usedRectangles[i].Height, y, y + height);
				if (usedRectangles[i].Y == y + height || usedRectangles[i].Y + usedRectangles[i].Height == y)
					score += CommonIntervalLength(usedRectangles[i].X, usedRectangles[i].X + usedRectangles[i].Width, x, x + width);
			}
			return score;
		}

		Rect MaxRectsBinPack::FindPositionForNewNodeContactPoint(int width, int height, int &bestContactScore) const
		{
			Rect bestNode;
//			memset(&bestNode, 0, sizeof(Rect));

			bestContactScore = -1;

			for (size_t i = 0; i < freeRectangles.size(); ++i)
			{
				// Try to place the rectangle in upright (non-flipped) orientation.
				if (freeRectangles[i].Width >= width && freeRectangles[i].Height >= height)
				{
					int score = ContactPointScoreNode(freeRectangles[i].X, freeRectangles[i].Y, width, height);
					if (score > bestContactScore)
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = width;
						bestNode.Height = height;
						bestContactScore = score;
					}
				}
				if (freeRectangles[i].Width >= height && freeRectangles[i].Height >= width)
				{
					int score = ContactPointScoreNode(freeRectangles[i].X, freeRectangles[i].Y, width, height);
					if (score > bestContactScore)
					{
						bestNode.X = freeRectangles[i].X;
						bestNode.Y = freeRectangles[i].Y;
						bestNode.Width = height;
						bestNode.Height = width;
						bestContactScore = score;
					}
				}
			}
			return bestNode;
		}

		bool MaxRectsBinPack::SplitFreeNode(Rect freeNode, const Rect &usedNode)
		{
			// Test with SAT if the rectangles even intersect.
			if (usedNode.X >= freeNode.X + freeNode.Width || usedNode.X + usedNode.Width <= freeNode.X ||
				usedNode.Y >= freeNode.Y + freeNode.Height || usedNode.Y + usedNode.Height <= freeNode.Y)
				return false;

			if (usedNode.X < freeNode.X + freeNode.Width && usedNode.X + usedNode.Width > freeNode.X)
			{
				// New node at the top side of the used node.
				if (usedNode.Y > freeNode.Y && usedNode.Y < freeNode.Y + freeNode.Height)
				{
					Rect newNode = freeNode;
					newNode.Height = usedNode.Y - newNode.Y;
					freeRectangles.push_back(newNode);
				}

				// New node at the bottom side of the used node.
				if (usedNode.Y + usedNode.Height < freeNode.Y + freeNode.Height)
				{
					Rect newNode = freeNode;
					newNode.Y = usedNode.Y + usedNode.Height;
					newNode.Height = freeNode.Y + freeNode.Height - (usedNode.Y + usedNode.Height);
					freeRectangles.push_back(newNode);
				}
			}

			if (usedNode.Y < freeNode.Y + freeNode.Height && usedNode.Y + usedNode.Height > freeNode.Y)
			{
				// New node at the left side of the used node.
				if (usedNode.X > freeNode.X && usedNode.X < freeNode.X + freeNode.Width)
				{
					Rect newNode = freeNode;
					newNode.Width = usedNode.X - newNode.X;
					freeRectangles.push_back(newNode);
				}

				// New node at the right side of the used node.
				if (usedNode.X + usedNode.Width < freeNode.X + freeNode.Width)
				{
					Rect newNode = freeNode;
					newNode.X = usedNode.X + usedNode.Width;
					newNode.Width = freeNode.X + freeNode.Width - (usedNode.X + usedNode.Width);
					freeRectangles.push_back(newNode);
				}
			}

			return true;
		}

		static bool IsContainedIn(const Rect& a, const Rect& b)
		{
			return a.X >= b.X && a.Y >= b.Y && a.X + a.Width <= b.X + b.Width && a.Y + a.Height <= b.Y + b.Height;
		}

		void MaxRectsBinPack::PruneFreeList()
		{
			/*
			///  Would be nice to do something like this, to avoid a Theta(n^2) loop through each pair.
			///  But unfortunately it doesn't quite cut it, since we also want to detect containment.
			///  Perhaps there's another way to do this faster than Theta(n^2).

			if (freeRectangles.size() > 0)
			clb::sort::QuickSort(&freeRectangles[0], freeRectangles.size(), NodeSortCmp);

			for(size_t i = 0; i < freeRectangles.size()-1; ++i)
			if (freeRectangles[i].x == freeRectangles[i+1].x &&
			freeRectangles[i].y == freeRectangles[i+1].y &&
			freeRectangles[i].width == freeRectangles[i+1].width &&
			freeRectangles[i].height == freeRectangles[i+1].height)
			{
			freeRectangles.erase(freeRectangles.begin() + i);
			--i;
			}
			*/

			/// Go through each pair and remove any rectangle that is redundant.
			for (size_t i = 0; i < freeRectangles.size(); ++i)
				for (size_t j = i + 1; j < freeRectangles.size(); ++j)
				{
					if (IsContainedIn(freeRectangles[i], freeRectangles[j]))
					{
						freeRectangles.erase(freeRectangles.begin() + i);
						--i;
						break;
					}
					if (IsContainedIn(freeRectangles[j], freeRectangles[i]))
					{
						freeRectangles.erase(freeRectangles.begin() + j);
						--j;
					}
				}
		}
	}
}