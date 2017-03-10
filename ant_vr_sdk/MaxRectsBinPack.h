/** @file MaxRectsBinPack.h
	@author Jukka Jylänki

	@brief Implements different bin packer algorithms that use the MAXRECTS data structure.

	This work is released to Public Domain, do whatever you want with it.
*/
#pragma once

#include <vector>
#include <memory>

namespace jet
{
	namespace util
	{
		typedef struct RectSize
		{
			int Width, Height;
			int UniqueID;

			RectSize(int w = 0, int h = 0, int u = -1) :
				Width(w), Height(h), UniqueID(u){}
		}RectSize;

		typedef struct Rect
		{
			int X, Y;
			int Width, Height;
			int UniqueID;
			bool Rotated;

			explicit Rect(RectSize& size)
			{
				X = 0;
				Y = 0;
				Width = size.Width;
				Height = size.Height;
				UniqueID = UniqueID;
				Rotated = false;
			}

#if 0
			Rect& operator=(Rect& o)
			{
				X = o.X;
				Y = o.Y;
				Width = o.Width;
				Height = o.Height;
				UniqueID = o.UniqueID;
				Rotated = o.Rotated;
			}
			
			explicit Rect(Rect& o)
			{
				X = o.X;
				Y = o.Y;
				Width = o.Width;
				Height = o.Height;
				UniqueID = o.UniqueID;
				Rotated = o.Rotated;
			}
#endif
			Rect() : UniqueID(-1), Rotated(false), X(0), Y(0), Width(0), Height(0){}

			bool isValid() const { return Width > 0 && Height > 0; }
		}Rect;

		typedef struct RectPage
		{
			int Width, Height;   // The size of the Page. 
			std::vector<Rect> OutputRects;
			std::vector<RectSize> RemainingRects;
			float Occupancy;

			RectPage(int _width = 0, int _height = 0) :
				Width(_width), Height(_height), Occupancy(0.0f)
			{}
		}RectPage;

		typedef std::shared_ptr<RectPage> RectPagePtr;
		/** MaxRectsBinPack implements the MAXRECTS data structure and different bin packing algorithms that
			use this structure. */
		class MaxRectsBinPack
		{
		public:
			/// Instantiates a bin of size (0,0). Call Init to create a new bin.
			MaxRectsBinPack();

			/// Instantiates a bin of the given size.
			MaxRectsBinPack(int width, int height);

			/// (Re)initializes the packer to an empty bin of width x height units. Call whenever
			/// you need to restart with a new bin.
			void Init(int width, int height);

			/// Specifies the different heuristic rules that can be used when deciding where to place a new rectangle.
			enum FreeRectChoiceHeuristic
			{
				RectBestShortSideFit, ///< -BSSF: Positions the rectangle against the short side of a free rectangle into which it fits the best.
				RectBestLongSideFit, ///< -BLSF: Positions the rectangle against the long side of a free rectangle into which it fits the best.
				RectBestAreaFit, ///< -BAF: Positions the rectangle into the smallest free rect into which it fits.
				RectBottomLeftRule, ///< -BL: Does the Tetris placement.
				RectContactPointRule ///< -CP: Choosest the placement where the rectangle touches other rects as much as possible.
			};

			/// Inserts the given list of rectangles in an offline/batch mode, possibly rotated.
			/// @param rects The list of rectangles to insert. This vector will be destroyed in the process.
			/// @param dst [out] This list will contain the packed rectangles. The indices will not correspond to that of rects.
			/// @param method The rectangle placement rule to use when packing.
			void Insert(std::vector<RectSize> &rects, std::vector<Rect> &dst, FreeRectChoiceHeuristic method);

			/// Inserts a single rectangle into the bin, possibly rotated.
			Rect Insert(int width, int height, int uid, FreeRectChoiceHeuristic method);

			RectPagePtr Pack(std::vector<RectSize> &rects, FreeRectChoiceHeuristic method);

			RectPagePtr GetResult();

			/// Computes the ratio of used surface area to the total bin area.
			float Occupancy() const;

		private:
			int binWidth;
			int binHeight;

			std::vector<Rect> usedRectangles;
			std::vector<Rect> freeRectangles;

			/// Computes the placement score for placing the given rectangle with the given method.
			/// @param score1 [out] The primary placement score will be outputted here.
			/// @param score2 [out] The secondary placement score will be outputted here. This isu sed to break ties.
			/// @return This struct identifies where the rectangle would be placed if it were placed.
			Rect ScoreRect(int width, int height, FreeRectChoiceHeuristic method, int &score1, int &score2) const;

			/// Places the given rectangle into the bin.
			void PlaceRect(const Rect &node);

			/// Computes the placement score for the -CP variant.
			int ContactPointScoreNode(int x, int y, int width, int height) const;

			Rect FindPositionForNewNodeBottomLeft(int width, int height, int &bestY, int &bestX) const;
			Rect FindPositionForNewNodeBestShortSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit) const;
			Rect FindPositionForNewNodeBestLongSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit) const;
			Rect FindPositionForNewNodeBestAreaFit(int width, int height, int &bestAreaFit, int &bestShortSideFit) const;
			Rect FindPositionForNewNodeContactPoint(int width, int height, int &contactScore) const;

			/// @return True if the free node was split.
			bool SplitFreeNode(Rect freeNode, const Rect &usedNode);

			/// Goes through the free rectangle list and removes any redundant entries.
			void PruneFreeList();
		};
	}
}