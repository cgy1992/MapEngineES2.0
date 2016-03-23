#include "ClusterLayer.h"
#include "DrawEngine.h"

ClusterLayer::ClusterLayer(enum VLayerType theLayerType, DrawEngine *den):
_interval(10)
, _reloadData(true)  //默认是 需要进行聚合分析
{
	_shader.initialize();
	// "C:\\Program Files (x86)\\Google\\Chrome\\Application\\simsun.ttc"  绝对路径
	_font.buildSystemFont("simsun.ttc",20);
	setLayerType(theLayerType);
	//默认设置 聚合点的大小
	_pointSize.push_back(20);
	_pointSize.push_back(40);
	_pointSize.push_back(60);
	this->den = den;
}

void ClusterLayer::loadDateFromJson(const Json::Value& dataInfo)
{
	int interval = dataInfo["Interval"].asInt();
	setInterval(interval);

	Json::Value compoundStyle = dataInfo["CompoundStyleId"];
	if (compoundStyle != Json::nullValue){
		for (auto i = 0; i < compoundStyle.size(); ++i){
			_compoundStyleId.push_back(compoundStyle[i].asInt());
		}
	}

	Json::Value pointSet = dataInfo["PointSet"];
	GeoPos pt;
	if (pointSet != Json::nullValue){
		for (int i = 0; i < pointSet.size(); ++i){
			Json::Value pos = pointSet[i];
			double geoX = pos["x"].asDouble();
			double geoY = pos["y"].asDouble();
			pt.setPos(geoX, geoY);
			addPoint(pt);
		}
	}

	//进行地图聚合分析
	if (_reloadData){
		clear();
		cluster();
		_reloadData = false;
	}
}
//求 两个点的空间距离
int ClusterLayer::distance(const GeoPos &pt1, const GeoPos &pt2){
	int res = -1;
	long pixX, pixY;

	den->Projection_to_Pixel(pt1.x_, pt1.y_, pixX, pixY);
	CELL::int2 pos1(pixX, pixY);

	den->Projection_to_Pixel(pt2.x_, pt2.y_, pixX, pixY);
	CELL::int2 pos2(pixX, pixY);

	return (res = CELL::distance(pos1, pos2));
}

//聚合分析
void ClusterLayer::cluster()
{
	auto iter1 = _dataSet.begin();
	auto iter2 = _dataSet.end();
	for (; iter1 != iter2; ++iter1) 
	{
		bool flag = true; //表示初始情况该item 需要插入
		for (auto t1 = _clusterDataSet.begin(); t1 != _clusterDataSet.end(); ++t1)
		{
			auto dis = distance(*iter1, t1->first); // iter1 原始数据  t1->first
			if (dis <= _interval){
				(t1->second).push_back(*iter1);
				//_compoundPointIndex[(t1->first).Id_] = t1;
				_isSpreadIndex[(t1->first).Id_] = false; //默认是 不散开
				flag = false;   
				break;
			}
		}
		if (flag){
			_clusterDataSet.push_back(make_pair(*iter1, vector<GeoPos>()));
			_compoundPointIndex[iter1->Id_] = _clusterDataSet.size() - 1;
		}
			
	}
	//按聚合点的数目分类
	auto b = _clusterDataSet.begin();
	auto e = _clusterDataSet.end();
	for (; b != e; ++b){
		_compoundCount.insert((b->second).size() + 1); 
		_compoundSort.insert(std::make_pair(((b->second).size() + 1), b->first));
	}
}

void ClusterLayer::Draw()
{

	_shader.begin();
	glUniformMatrix4fv(_shader._MVP, 1, GL_FALSE, (GLfloat*)den->g_mpv);

		if (_reloadData){ //鼠标wheel 滚动后
			clear();
			cluster();
			_reloadData = false;
		}

		auto iter1 = _compoundCount.begin();
		auto iter2 = _compoundCount.end();
		//分 3个级别 去绘制聚合点
		for (; iter1 != iter2; ++iter1)
		{
			if (*iter1 < SMALL){ //单点
				if (!(den->g_vecStyle).count(_compoundStyleId[0]))
					MessageBox(NULL, _T("unregister sytleID"), _T("Error!"),
					MB_ICONEXCLAMATION | MB_OK);
				CMarkerStyle *style = (CMarkerStyle*)((den->g_vecStyle)[_compoundStyleId[0]]);
				DrawBySort(_pointSize[0],*iter1,style->m_textureID);
			}
			if (*iter1 >= SMALL && *iter1 < MIDDLE){  //  < 10个点
				if (!(den->g_vecStyle).count(_compoundStyleId[1]))
					MessageBox(NULL, _T("unregister sytleID"), _T("Error!"),
					MB_ICONEXCLAMATION | MB_OK);
				CMarkerStyle *style = (CMarkerStyle*)((den->g_vecStyle)[_compoundStyleId[1]]);
				DrawBySort(_pointSize[1], *iter1,style->m_textureID);
			}
			if (*iter1 >= MIDDLE){ // 大于10个点的
				if (!(den->g_vecStyle).count(_compoundStyleId[2]))
					MessageBox(NULL, _T("unregister sytleID"), _T("Error!"),
					MB_ICONEXCLAMATION | MB_OK);
				CMarkerStyle *style = (CMarkerStyle*)((den->g_vecStyle)[_compoundStyleId[2]]);
				DrawBySort(_pointSize[2], *iter1,style->m_textureID);
			}
		}

	_shader.end();
	//渲染文字
	_font.beginText((float *)den->g_mpv);

		for (const auto &item : _clusterDataSet){
			if (0 != item.second.size()){
				auto cntNum = item.second.size() + 1;
				long pix, piy;
				den->Projection_to_Pixel(item.first.x_, item.first.y_, pix, piy);
				char strNum[10];
				itoa(cntNum,strNum,10);
				wchar_t * str = CharToWchar(strNum);
				_font.drawText(pix-8 , piy-10 , 0, CELL::Rgba4Byte(255, 255, 100, 200), str, -1);				
				delete[] str;
			}
		}

	_font.endText();
}

void ClusterLayer::DrawBySort(int pointSize, int clusterCount, GLuint textureId)
{
	//采用多级纹理来显示点上面的数字

		glUniform1f(_shader._pointSize, pointSize);
		glBindTexture(GL_TEXTURE_2D, textureId); //默认是启动第一阶段纹理
		glUniform1i(_shader._texture, 0); //使用第一阶段的纹理
		auto range = _compoundSort.equal_range(clusterCount); //按照 聚合数目分类
		vector<ShPoint> vertex; //绘制时，使用的顶点数据
		for (; range.first != range.second; ++range.first)
		{
			//保存基准点
			PixPoint p_tmp;
			den->Projection_to_Pixel(((range.first)->second).x_, ((range.first)->second).y_,p_tmp.x_, p_tmp.y_);
			ShPoint tt; //供着色器使用的 float格式
			tt.x_ = p_tmp.x_;
			tt.y_ = p_tmp.y_;
			vertex.push_back(tt);
		}
		//开始绘制，一定要使用GL_FLOAT ，使用int数据类型 会导致绘不出来
		glVertexAttribPointer(_shader._position, 2, GL_FLOAT, false, sizeof(ShPoint), &vertex[0]);
		glDrawArrays(GL_POINTS, 0, vertex.size());
	
}

//搜索的对象 针对 _clusterDataSet
vector<int> ClusterLayer::searchByPoint(PP_Point& mousePoint)
{
	vector<int> res ;
	auto iter1 = _clusterDataSet.begin();
	auto iter2 = _clusterDataSet.end();

	POINT tmp;
	tmp.x = mousePoint.x;
	tmp.y = mousePoint.y;
	RECT rect;
	for (; iter1 != iter2; ++iter1){
		long pix = 0;
		long piy = 0;
		den->Projection_to_Pixel(iter1->first.x_, iter1->first.y_,pix, piy);
		//该点 在窗口范围内
		if (pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0){
			PixPoint currentPos;
			currentPos.setPos(pix, piy);
			setRectByClusterCount(currentPos,iter1->second.size(), rect); //不同的聚合点数 矩形框大小不同
			if (PtInRect(&rect, tmp))
			{
				res.push_back(iter1->first.Id_);
				if (iter1->second.size() > 0){ //表示该点的聚合
					for (const GeoPos& item : iter1->second){
						res.push_back(item.Id_);
					}
				}

				break;
			}
		}
	}
	return res;
}

//鼠标移动到聚合点上 改变样式
bool ClusterLayer::getRgn(CRgn &pointArea)
{
	bool bRes = false;
	std::vector<POINT> pointSet;
	POINT tmp;
	for (const ClusterPoint &item : _clusterDataSet){
		const GeoPos& currentGeoPos = item.first;
		long pix = 0;
		long piy = 0;
		den->Projection_to_Pixel(currentGeoPos.x_, currentGeoPos.y_, pix, piy);
		if (pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0){
			tmp.x = pix - _pointSize[1] / 2;
			tmp.y = piy - _pointSize[1] / 2;
			pointSet.push_back(tmp);

			tmp.x = pix + _pointSize[1] / 2;
			tmp.y = piy - _pointSize[1] / 2;
			pointSet.push_back(tmp);

			tmp.x = pix + _pointSize[1] / 2;
			tmp.y = piy + _pointSize[1] / 2;
			pointSet.push_back(tmp);

			tmp.x = pix - _pointSize[1] / 2;
			tmp.y = piy + _pointSize[1] / 2;
			pointSet.push_back(tmp);
		}
	}
	if (pointSet.empty())
		return bRes;
	int numb = pointSet.size() / 4;
	int* numberPolygon = new int[numb];
	for (int i = 0; i < numb; i++)
		numberPolygon[i] = 4;
	bRes = pointArea.CreatePolyPolygonRgn(&(pointSet[0]), numberPolygon, numb, WINDING);
	return bRes;

}