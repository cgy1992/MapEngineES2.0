#include "stdafx.h"
#include "MapUrl.h"

void MapUrl::GetMapLocations(const int& js, const double& leftx, const double& lefty
					, const double& rightx, const double& righty
					, std::vector<MapTileLocation>& theLocation)
{
	MapTileLocation theCurrentLocation;
	theCurrentLocation.js_ = js;
	theLocation.clear();
	double theCurrentReloution = m_mapReloution_[js];
	double eachTileColReloution = theCurrentReloution * m_TileCols_;
	double eachTileRowReloution = theCurrentReloution * m_TileRows_;

	int leftxNumber = floor((leftx+180) / eachTileRowReloution);
	int rightxNumber = floor((rightx+180) / eachTileRowReloution);
	int leftyNumber = floor((lefty+270) / eachTileColReloution);
	int rightyNumber = floor((righty+270) / eachTileColReloution);


	int havConnectNumber = 0;
	bool bWaiteData = false;
	for(int beginx = leftxNumber; beginx <= rightxNumber; beginx++)
	{
		theCurrentLocation.Col_ = beginx;
		for(int beginy = rightyNumber; beginy <= leftyNumber; beginy++)
		{
			theCurrentLocation.Row_ = beginy;
			theLocation.push_back(theCurrentLocation);
		}
	}
}