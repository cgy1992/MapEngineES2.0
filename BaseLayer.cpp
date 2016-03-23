#include "BaseLayer.h"


CBaseLayer::CBaseLayer(void)
{
	//memset(&m_LayerName,0,sizeof(m_LayerName));
	m_MapType = MapType::VtronMapTile;
	m_layerType = VLayerType::vBaseLayer;
	m_MaxLevel = -1;
	m_MinLevel = -1;
}

//只用释放掉m_Cachemap中的tile，其他两个容器中tile* 都是这个中的副本,释放地图资源
CBaseLayer::~CBaseLayer(void)
{
	auto iter1 = m_Cachemap.begin();
	auto iter2 = m_Cachemap.end();
	while(iter1 != iter2)
	{
	  if(iter1->second)
	  {
		  delete iter1->second;
		  iter1->second = NULL;
	  }
	  ++iter1;
	}
	m_Cachemap.clear();
}


void CBaseLayer::Draw()
{

}

void CBaseLayer::SetMapCentre(double CenX,double CenY,double CenZ,int js)
{
	m_x = CenX;
	m_y = CenY,
	//m_z = CenZ
	m_js = js;
	m_bUpdateData = true;
}

void CBaseLayer::GetMapCentre(double &CenX,double &CenY,double &CenZ,int &js)
{
	CenX = m_x;
	CenY = m_y;
	CenZ = 1;
	js = m_js;
}

void CBaseLayer::GetBaseLayerConf(double &Cenlon,double &Cenlat,double & width,double & height
	,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution)
{
	Cenlon = m_CenLon;
	Cenlat = m_CenLat;
	width = m_Scale2ImageW;
	height = m_Scale2ImageH;//这个值什么时候初始化的？？
	leftlon = m_LeftTop_Lon;
	Toplat = m_LeftTop_Lat;
	lon = m_Lon;
	lat  = m_Lat;
	scale = m_Scale;
	Path = m_MapName;
	resolution = m_resolution;
};

void CBaseLayer::DecodeTile()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD start = GetTickCount();
	HANDLE hdl[g_countT];
	hdl[0]= CreateThread(NULL,NULL,&Datathread1,  (LPVOID)this,NULL,NULL);
	hdl[1]=CreateThread(NULL,NULL,&Datathread2,  (LPVOID)this,NULL,NULL);
	hdl[2]=CreateThread(NULL,NULL,&Datathread3,  (LPVOID)this,NULL,NULL);
	hdl[3]=CreateThread(NULL,NULL,&Datathread4,  (LPVOID)this,NULL,NULL);
	hdl[4]=CreateThread(NULL,NULL,&Datathread5,  (LPVOID)this,NULL,NULL);
	hdl[5]=CreateThread(NULL,NULL,&Datathread6,  (LPVOID)this,NULL,NULL);
	Sleep(0);
	WaitForMultipleObjects(g_countT,hdl,TRUE,INFINITE); //阻塞，等待所有子线程完成
	DWORD end = GetTickCount() - start;
}

DWORD WINAPI  CBaseLayer::Datathread1(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;

		if (id==(object->m_tmpData).size()/g_countT)
		{
			break;
		}

		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		tile->m_pdata = NULL;

		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CBaseLayer::Datathread2(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<(object->m_tmpData).size()*1/g_countT)
		{		
			continue;
		}
		if (id==(object->m_tmpData).size()*2/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;
		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CBaseLayer::Datathread3(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<(object->m_tmpData).size()*2/g_countT)
		{
			continue;
		}
		if (id==(object->m_tmpData).size()*3/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CBaseLayer::Datathread4(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<(object->m_tmpData).size()*3/g_countT)
		{		
			continue;
		}
		if (id==(object->m_tmpData).size()*4/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CBaseLayer::Datathread5(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<(object->m_tmpData).size()*4/g_countT)
		{	
			continue;
		}
		if (id==(object->m_tmpData).size()*5/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}


		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CBaseLayer::Datathread6(LPVOID t)
{
	CBaseLayer *object = (CBaseLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<(object->m_tmpData).size()*5/g_countT)
		{		
			continue;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==0)
		{
			dep = 3;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}

//主线程解码
void  CBaseLayer::decodetileSingleT()
{
	CBaseLayer *object = (CBaseLayer*)this;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();

	for(;it!=(object->m_tmpData).end();it++)
	{	
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;

		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			continue;
		}
		int dep = image.GetBPP()/8;	
		int bpp = image.GetBPP();
		BYTE *rgbdata = NULL;
		if (dep==0)
		{
			dep = 1;
			tile->m_sformat = GL_RGB;
			rgbdata = new BYTE[w*h*3];
			memset(rgbdata,255,w*h*3);
		}
		else if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}
		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			memset(pdata,0,w*h*dep);
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];

				if (bpp == 1)
				{
					rgbdata[i*3+0] = 204; //r
					rgbdata[i*3+1] = 204; //g
					rgbdata[i*3+2] = 204; //b
				}
				else
				{
					rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
					rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
					rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
					//rgbdata[i*3+3] = 255; //颜色索引的A值固定
				}			
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return;
}