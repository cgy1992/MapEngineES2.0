#pragma once
#include "stdafx.h"
struct MapTileLocation
{
	long js_;
	long Col_;
	long Row_;
};
class MapUrl
{
private:
	int m_TileCols_;
	int m_TileRows_;
	std::map<long, double> m_mapReloution_;
public:
	MapUrl()
		: m_TileCols_(256)
		, m_TileRows_(256)
	{
		double curReloution = 2;
		for(int i = 0; i < 23; i++)
		{
			m_mapReloution_[i] = curReloution;
			curReloution /= 2;
		}
	}
	~MapUrl()
	{}


	void GetMapLocations(const int& js, const double& leftx, const double& lefty
					, const double& rightx, const double& righty
					, std::vector<MapTileLocation>& theLocation);

};