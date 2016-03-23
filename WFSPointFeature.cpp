#include "WFSPointFeature.h"
#include <map>
#include <utility>
//#define DEBUGGPS
extern std::map<int,CStyle*> g_vecStyle; 

CWFSPointFeatureElementSet::CWFSPointFeatureElementSet(DrawEngine *pden)
	: m_FeatureHandle_(-1),m_ModifyStyle_(false),m_query_show(false),m_query_hide(false),m_IsBus(false)  //modify by xuyan 初始未修改过
	  ,m_addStyle(false),den(pden),theElement(NULL)
{
	theElement = new CMarkerElement(den);
	//m_DataSet_.clear();  //默认构造的就是空容器
}

CWFSPointFeatureElementSet::~CWFSPointFeatureElementSet()
{
	RemoveAllFeature();
	if(theElement)
	{
	  delete theElement;
	  theElement = NULL;
	}
}
void CWFSPointFeatureElementSet::RemoveAllFeature()
{
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_DataSet_.end();
	while(theBegin != theEnd)
	{
		delete theBegin->second;
		theBegin++;
	}
	m_DataSet_.clear();
	m_FeatureHandle_ = -1;
}

long CWFSPointFeatureElementSet::AddOneFeature(WFSPointFeatureElement* pFeature)
{
	m_FeatureHandle_++;
	m_DataSet_[m_FeatureHandle_] = pFeature;
	return m_FeatureHandle_;
}

//按照theStyle的样式风格来 绘制[热点]
void CWFSPointFeatureElementSet::RenderManyUseOgles(CMarkerStyle* theStyle,DrawEngine *den)
{
	int errorNum = 0;
	DWORD dwStart = GetTickCount();  
	//首先判断是否是 查询的结果集
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_query_show ? m_QueryDataSet_.begin():m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd =m_query_show ? m_QueryDataSet_.end():m_DataSet_.end();
	std::vector<Vertex> theVertexVector; //保存 纹理贴图 几何图元的顶点信息
	theElement->m_markerstyle = theStyle; //设定贴图的默认样式 ,该样式是CWFSLayer中的成员变量
	//遍历每一个热点
	int cnt = 0;
	int discard = 0;
	while (theBegin != theEnd)
	{
		//如果是 隐藏的点 ，就跳过
		if(m_query_hide && m_HidePointSet_.find(theBegin->first) != m_HidePointSet_.end())
		{
			theBegin ++;
			continue;
		}
		WFSPointFeatureElement* theCurrentFeature = theBegin->second;
		//此点修改过 样式
		if(NULL != theCurrentFeature->m_markerstyle) 
		{
		  theBegin++; //跳过该点处理
		  continue;  //修改了样式的热点不render
		}
		//1个点-->6个点  1个点-->2个三角形
		ComposeVertex(theCurrentFeature,theVertexVector,den,discard);
		theBegin++;
		++cnt; //统计所有
	}
	theElement->RenderManyUseOgles(theVertexVector); 
#ifdef DEBUGGPS
	errorNum = glGetError();
	char tex_info[100] = {0}; //旧的日志信息
	if(difftime(den->now,den->pre) >= 5) //超过5s
	{
		memset(tex_info,0,strlen(tex_info));
		sprintf(tex_info,"\tunmodify_trangle --> %d个 glGetError --> %d  discard_bus_num is --> %d original_bus_num-->%d",theVertexVector.size()/3,errorNum,discard,cnt);
		fwrite(tex_info,strlen(tex_info),1,tex_log);
	}
#endif
	theVertexVector.clear();  //清空容器，供下一次使用
	if(m_ModifyStyle_) //接下来，重新遍历每一个 热点，以点为单位，重新的贴纹理图片
	{
		int sumTrangle = 0;
		int cnt = 0;
		int discard = 0;
		//std::vector<Vertex> theVertexVector; //重用上面定义的容器
		for(auto begin = m_Style_.begin();begin != m_Style_.end();++begin) //按style的种类绘制
		{
			//设置新的纹理
			theElement->m_markerstyle = (CMarkerStyle *)den->g_vecStyle[*begin];
			//遍历使用该类型的所有 FID
			typedef std::multimap<int,long>::iterator fids_it;
			std::pair<fids_it,fids_it> range= m_ModifySet_.equal_range(*begin);
			while(range.first != range.second) //遍历使用同种style的FID
			{
				//这里包含了，那些点 显示的判断
				if(m_query_show && 0 == m_QueryDataSet_.count(range.first->second))
				{
					range.first++;
					continue;
				}
				if(m_query_hide && 1 == m_HidePointSet_.count(range.first->second))
				{
					range.first++;
					continue;
				}
				WFSPointFeatureElement* theCurrentFeature = m_DataSet_[range.first->second];  //读取当前热点
				//由WFSPoint的
				
				ComposeVertex(theCurrentFeature,theVertexVector,den,discard);
				++range.first;
			}//内层while结束
			theElement->RenderManyUseOgles(theVertexVector);  //绘制这一批 使用同种style的FID
			sumTrangle += theVertexVector.size()/3;//用于输出的三角片的个数
			theVertexVector.clear(); //清空供下一次使用
		 }
#ifdef DEBUGGPS
		errorNum = glGetError(); //opengl错误码
		if(difftime(den->now,den->pre) >= 5)//间隔5s输出一次
		{
			memset(tex_info,0,strlen(tex_info));
			sprintf(tex_info,"\tmodify trangle --> %d个 discard--> %d个 glGetError --> %d modify style bus's num --> %d  ",sumTrangle,discard,errorNum,m_ModifySet_.size());
			fwrite(tex_info,strlen(tex_info),1,tex_log);
		}
#endif
	} //if（modify）结束
/***************************在这里继续叠加车牌号******************************************/
	if(m_IsBus && m_addStyle)  //WFSPoint是车辆，并且增删过样式
	{
		int sumTrangle = 0;
		//使用过叠加的样式
		auto iter_set = m_append_style.begin();
		while(iter_set != m_append_style.end())
		{
			theElement->m_markerstyle = (CMarkerStyle *)den->g_vecStyle[*iter_set];//得到新纹理
			//开始render 使用该样式 的所有WFSPoint
			typedef std::multimap<int,StylePointID>::iterator fids_it;
			std::pair<fids_it,fids_it> range= m_styleID_to_Fid.equal_range(*iter_set);
			while(range.first != range.second)
			{
				//这里包含了，那些点 显示的判断
				if(m_query_show && 0 == m_QueryDataSet_.count(range.first->second.fid))
				{
					range.first++;
					continue;
				}
				if(m_query_hide && 1 == m_HidePointSet_.count(range.first->second.fid))
				{
					range.first++;
					continue;
				}
				//组点，内部会依据图层的显示级别，来辨别是否显示某样式
				ComposeVertex(range.first->second,theVertexVector,den); 
				++range.first;
			}//同种类的百位while结束
		    theElement->RenderManyUseOgles(theVertexVector);
			sumTrangle += theVertexVector.size()/3;
			++iter_set; //绘制下一个样式
			theVertexVector.clear(); //清空供下一次 绘制使用
		}
#ifdef DEBUGGPS
		errorNum = glGetError();
		if(difftime(den->now,den->pre) >= 5) //时差间隔5s
		{
			memset(tex_info,0,strlen(tex_info));
			sprintf(tex_info,"add_style_trangle --> %d个 glGetError --> %d  add_style's num is --> %d   ",sumTrangle,errorNum,m_styleID_to_Fid.size());
			fwrite(tex_info,strlen(tex_info),1,tex_log);
		}
#endif
	} //叠加过样式&&是汽车层 if结束
#ifdef DEBUGGPS
	DWORD dwEnd = GetTickCount();
	if(difftime(den->now,den->pre) >= 5)
	{
		memset(tex_info,0,strlen(tex_info));
		sprintf(tex_info,"render %d ms the bus --> %d\r\n",(dwEnd-dwStart),m_DataSet_.size());
		fwrite(tex_info,strlen(tex_info),1,tex_log);
		fflush(tex_log);
		den->pre = den->now; //下一次间隔5S打印日志
		dwStart = dwEnd;
	}
#endif
}

//输入样式的标识符，组合样式的顶点
void CWFSPointFeatureElementSet::ComposeVertex(const StylePointID &Fid,std::vector<Vertex> &theVertexVector,DrawEngine *den)
{
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;
	WFSPointFeatureElement* theCurrentFeature = m_DataSet_[Fid.fid];  //读取热点
	long pix = 0;
	long piy = 0;
	den->Projection_to_Pixel(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
	//theCurrentFeature->m_show_flagArr[den->m_offset]? theCurrentFeature->m_visible = true:theCurrentFeature->m_visible = false;
	//需要对m_offset进行预处理
	int sizeflag = theCurrentFeature->m_show_flagArr[Fid].size();
	if(sizeflag == 0)
		return;
	if(den->m_offset >= sizeflag)
		den->m_offset = sizeflag - 1;
	else if(den->m_offset < 0)
		den->m_offset = 0;
	//车辆热点可见
	if(theCurrentFeature->m_visible &&pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentFeature->m_show_flagArr[Fid][den->m_offset])
	{
	   //叠加样式的的中心点
			pix = pix + theCurrentFeature->m_styleID_to_pos[Fid].left;
			piy = piy + theCurrentFeature->m_styleID_to_pos[Fid].top;

			verts[0].tu = 0;
			verts[0].tv = 1;
			verts[0].loc[0] = pix - theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[0].loc[1] = piy + theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[0].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[0]);
			//1
			verts[1].tu = 0;
			verts[1].tv = 0;
			verts[1].loc[0] = pix - theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[1].loc[1] = piy - theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[1].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[1]);
			//2
			verts[2].tu = 1;
			verts[2].tv = 1;
			verts[2].loc[0] = pix + theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[2].loc[1] = piy + theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[2].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[2]);
			//2
			verts[3].tu = 1;
			verts[3].tv = 1;
			verts[3].loc[0] = pix + theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[3].loc[1] = piy + theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[3].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[3]);
			//3
			verts[4].tu = 0;
			verts[4].tv = 0;
			verts[4].loc[0] = pix - theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[4].loc[1] = piy - theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[4].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[4]);
			//2
			verts[5].tu = 1;
			verts[5].tv = 0;
			verts[5].loc[0] = pix + theCurrentFeature->m_styleID_to_pos[Fid].width / 2;
			verts[5].loc[1] = piy - theCurrentFeature->m_styleID_to_pos[Fid].height / 2;
			verts[5].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[5]);
	}
}

//重载这个组点函数，现在由一个WFSPointFeatureElement组和纹理贴图的几何图元顶点
void CWFSPointFeatureElementSet::ComposeVertex(const WFSPointFeatureElement *theCurrentFeature,std::vector<Vertex> &theVertexVector,DrawEngine *den,int &discardPoint)
{
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	//透明度 均设置为1
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;
	long pix = 0;
	long piy = 0;
	den->Projection_to_Pixel(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
	//g_CtrlW是窗口的 宽  g_CtrlH是窗口的高
	//如果 这个点 在我们的窗口的可视范围内 才绘制这个 点
	if(pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentFeature->m_visible)  
	//这个成员变量在CElement's constructor中初始为 TRUE，可以忽略
	{
		//这里的 m_rendW 与 m_rendH 是由JS 加载WFSPoint层时 发送过来
		verts[0].tu = 0;
		verts[0].tv = 1;
		verts[0].loc[0] = pix - theCurrentFeature->m_rendW / 2;
		verts[0].loc[1] = piy + theCurrentFeature->m_rendH / 2;
		verts[0].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[0]);
		//1
		verts[1].tu = 0;
		verts[1].tv = 0;
		verts[1].loc[0] = pix - theCurrentFeature->m_rendW / 2;
		verts[1].loc[1] = piy - theCurrentFeature->m_rendH / 2;
		verts[1].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[1]);
		//2
		verts[2].tu = 1;
		verts[2].tv = 1;
		verts[2].loc[0] = pix + theCurrentFeature->m_rendW / 2;
		verts[2].loc[1] = piy + theCurrentFeature->m_rendH / 2;
		verts[2].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[2]);

		//2
		verts[3].tu = 1;
		verts[3].tv = 1;
		verts[3].loc[0] = pix + theCurrentFeature->m_rendW / 2;
		verts[3].loc[1] = piy + theCurrentFeature->m_rendH / 2;
		verts[3].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[3]);
		//3
		verts[4].tu = 0;
		verts[4].tv = 0;
		verts[4].loc[0] = pix - theCurrentFeature->m_rendW / 2;
		verts[4].loc[1] = piy - theCurrentFeature->m_rendH / 2;
		verts[4].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[4]);
		//2
		verts[5].tu = 1;
		verts[5].tv = 0;
		verts[5].loc[0] = pix + theCurrentFeature->m_rendW / 2;
		verts[5].loc[1] = piy - theCurrentFeature->m_rendH / 2;
		verts[5].loc[2] = g_Zlocation;
		theVertexVector.push_back(verts[5]);
	}
	else
	{
		++discardPoint; //不在范围内，丢弃的点
	}
}