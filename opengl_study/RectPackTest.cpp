#include <MaxRectsBinPack.h>
#include <stdio.h>

using namespace jet::util;

void rect_pack_test()
{
	std::vector<RectSize> rects;
	std::vector<Rect> dst;
	rects.push_back(RectSize(40, 125, 0));
	rects.push_back(RectSize(140, 225, 1));
	rects.push_back(RectSize(80, 225, 2));
	rects.push_back(RectSize(60, 225, 3));
	rects.push_back(RectSize(20, 200, 4));
	rects.push_back(RectSize(100, 400, 5));
	rects.push_back(RectSize(90, 360, 6));
	rects.push_back(RectSize(450, 500, 7));
	rects.push_back(RectSize(360, 250, 8));
	rects.push_back(RectSize(280, 50, 9));

	MaxRectsBinPack rectPack;
	rectPack.Init(2048, 2048);

	rectPack.Insert(rects, dst, MaxRectsBinPack::RectBestAreaFit);

	RectPagePtr result = rectPack.GetResult();
	printf(".....\n");

}