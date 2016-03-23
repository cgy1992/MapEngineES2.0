#include "HeatMap.h"
#include "DrawEngine.h"

void HeatMap::addPoint(const HeatPoint &point)
{
	long pixX,pixY;
	den->Projection_to_Pixel(point.x,point.y,pixX,pixY);
	this->addPoint(pixX,pixY,point.size,point.intensity);
}

void HeatMap::addPoints(const std::vector<HeatPoint> &pointSet)
{
	auto iter1 = pointSet.begin();
	auto iter2 = pointSet.end();
	for(;iter1 != iter2;++iter1){
		long pixX,pixY;
		den->Projection_to_Pixel(iter1->x,iter1->y,pixX,pixY);
		this->addPoint(pixX,pixY,iter1->size,iter1->intensity);
	}
}

