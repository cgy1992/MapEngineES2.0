#include "WFSPointFeature.h"
#include <map>
#include <utility>
//#define DEBUGGPS
extern std::map<int,CStyle*> g_vecStyle; 

CWFSPointFeatureElementSet::CWFSPointFeatureElementSet(DrawEngine *pden)
	: m_FeatureHandle_(-1),m_ModifyStyle_(false),m_query_show(false),m_query_hide(false),m_IsBus(false)  //modify by xuyan ��ʼδ�޸Ĺ�
	  ,m_addStyle(false),den(pden),theElement(NULL)
{
	theElement = new CMarkerElement(den);
	//m_DataSet_.clear();  //Ĭ�Ϲ���ľ��ǿ�����
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

//����theStyle����ʽ����� ����[�ȵ�]
void CWFSPointFeatureElementSet::RenderManyUseOgles(CMarkerStyle* theStyle,DrawEngine *den)
{
	int errorNum = 0;
	DWORD dwStart = GetTickCount();  
	//�����ж��Ƿ��� ��ѯ�Ľ����
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_query_show ? m_QueryDataSet_.begin():m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd =m_query_show ? m_QueryDataSet_.end():m_DataSet_.end();
	std::vector<Vertex> theVertexVector; //���� ������ͼ ����ͼԪ�Ķ�����Ϣ
	theElement->m_markerstyle = theStyle; //�趨��ͼ��Ĭ����ʽ ,����ʽ��CWFSLayer�еĳ�Ա����
	//����ÿһ���ȵ�
	int cnt = 0;
	int discard = 0;
	while (theBegin != theEnd)
	{
		//����� ���صĵ� ��������
		if(m_query_hide && m_HidePointSet_.find(theBegin->first) != m_HidePointSet_.end())
		{
			theBegin ++;
			continue;
		}
		WFSPointFeatureElement* theCurrentFeature = theBegin->second;
		//�˵��޸Ĺ� ��ʽ
		if(NULL != theCurrentFeature->m_markerstyle) 
		{
		  theBegin++; //�����õ㴦��
		  continue;  //�޸�����ʽ���ȵ㲻render
		}
		//1����-->6����  1����-->2��������
		ComposeVertex(theCurrentFeature,theVertexVector,den,discard);
		theBegin++;
		++cnt; //ͳ������
	}
	theElement->RenderManyUseOgles(theVertexVector); 
#ifdef DEBUGGPS
	errorNum = glGetError();
	char tex_info[100] = {0}; //�ɵ���־��Ϣ
	if(difftime(den->now,den->pre) >= 5) //����5s
	{
		memset(tex_info,0,strlen(tex_info));
		sprintf(tex_info,"\tunmodify_trangle --> %d�� glGetError --> %d  discard_bus_num is --> %d original_bus_num-->%d",theVertexVector.size()/3,errorNum,discard,cnt);
		fwrite(tex_info,strlen(tex_info),1,tex_log);
	}
#endif
	theVertexVector.clear();  //�������������һ��ʹ��
	if(m_ModifyStyle_) //�����������±���ÿһ�� �ȵ㣬�Ե�Ϊ��λ�����µ�������ͼƬ
	{
		int sumTrangle = 0;
		int cnt = 0;
		int discard = 0;
		//std::vector<Vertex> theVertexVector; //�������涨�������
		for(auto begin = m_Style_.begin();begin != m_Style_.end();++begin) //��style���������
		{
			//�����µ�����
			theElement->m_markerstyle = (CMarkerStyle *)den->g_vecStyle[*begin];
			//����ʹ�ø����͵����� FID
			typedef std::multimap<int,long>::iterator fids_it;
			std::pair<fids_it,fids_it> range= m_ModifySet_.equal_range(*begin);
			while(range.first != range.second) //����ʹ��ͬ��style��FID
			{
				//��������ˣ���Щ�� ��ʾ���ж�
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
				WFSPointFeatureElement* theCurrentFeature = m_DataSet_[range.first->second];  //��ȡ��ǰ�ȵ�
				//��WFSPoint��
				
				ComposeVertex(theCurrentFeature,theVertexVector,den,discard);
				++range.first;
			}//�ڲ�while����
			theElement->RenderManyUseOgles(theVertexVector);  //������һ�� ʹ��ͬ��style��FID
			sumTrangle += theVertexVector.size()/3;//�������������Ƭ�ĸ���
			theVertexVector.clear(); //��չ���һ��ʹ��
		 }
#ifdef DEBUGGPS
		errorNum = glGetError(); //opengl������
		if(difftime(den->now,den->pre) >= 5)//���5s���һ��
		{
			memset(tex_info,0,strlen(tex_info));
			sprintf(tex_info,"\tmodify trangle --> %d�� discard--> %d�� glGetError --> %d modify style bus's num --> %d  ",sumTrangle,discard,errorNum,m_ModifySet_.size());
			fwrite(tex_info,strlen(tex_info),1,tex_log);
		}
#endif
	} //if��modify������
/***************************������������ӳ��ƺ�******************************************/
	if(m_IsBus && m_addStyle)  //WFSPoint�ǳ�����������ɾ����ʽ
	{
		int sumTrangle = 0;
		//ʹ�ù����ӵ���ʽ
		auto iter_set = m_append_style.begin();
		while(iter_set != m_append_style.end())
		{
			theElement->m_markerstyle = (CMarkerStyle *)den->g_vecStyle[*iter_set];//�õ�������
			//��ʼrender ʹ�ø���ʽ ������WFSPoint
			typedef std::multimap<int,StylePointID>::iterator fids_it;
			std::pair<fids_it,fids_it> range= m_styleID_to_Fid.equal_range(*iter_set);
			while(range.first != range.second)
			{
				//��������ˣ���Щ�� ��ʾ���ж�
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
				//��㣬�ڲ�������ͼ�����ʾ����������Ƿ���ʾĳ��ʽ
				ComposeVertex(range.first->second,theVertexVector,den); 
				++range.first;
			}//ͬ����İ�λwhile����
		    theElement->RenderManyUseOgles(theVertexVector);
			sumTrangle += theVertexVector.size()/3;
			++iter_set; //������һ����ʽ
			theVertexVector.clear(); //��չ���һ�� ����ʹ��
		}
#ifdef DEBUGGPS
		errorNum = glGetError();
		if(difftime(den->now,den->pre) >= 5) //ʱ����5s
		{
			memset(tex_info,0,strlen(tex_info));
			sprintf(tex_info,"add_style_trangle --> %d�� glGetError --> %d  add_style's num is --> %d   ",sumTrangle,errorNum,m_styleID_to_Fid.size());
			fwrite(tex_info,strlen(tex_info),1,tex_log);
		}
#endif
	} //���ӹ���ʽ&&�������� if����
#ifdef DEBUGGPS
	DWORD dwEnd = GetTickCount();
	if(difftime(den->now,den->pre) >= 5)
	{
		memset(tex_info,0,strlen(tex_info));
		sprintf(tex_info,"render %d ms the bus --> %d\r\n",(dwEnd-dwStart),m_DataSet_.size());
		fwrite(tex_info,strlen(tex_info),1,tex_log);
		fflush(tex_log);
		den->pre = den->now; //��һ�μ��5S��ӡ��־
		dwStart = dwEnd;
	}
#endif
}

//������ʽ�ı�ʶ���������ʽ�Ķ���
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
	WFSPointFeatureElement* theCurrentFeature = m_DataSet_[Fid.fid];  //��ȡ�ȵ�
	long pix = 0;
	long piy = 0;
	den->Projection_to_Pixel(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
	//theCurrentFeature->m_show_flagArr[den->m_offset]? theCurrentFeature->m_visible = true:theCurrentFeature->m_visible = false;
	//��Ҫ��m_offset����Ԥ����
	int sizeflag = theCurrentFeature->m_show_flagArr[Fid].size();
	if(sizeflag == 0)
		return;
	if(den->m_offset >= sizeflag)
		den->m_offset = sizeflag - 1;
	else if(den->m_offset < 0)
		den->m_offset = 0;
	//�����ȵ�ɼ�
	if(theCurrentFeature->m_visible &&pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentFeature->m_show_flagArr[Fid][den->m_offset])
	{
	   //������ʽ�ĵ����ĵ�
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

//���������㺯����������һ��WFSPointFeatureElement���������ͼ�ļ���ͼԪ����
void CWFSPointFeatureElementSet::ComposeVertex(const WFSPointFeatureElement *theCurrentFeature,std::vector<Vertex> &theVertexVector,DrawEngine *den,int &discardPoint)
{
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	//͸���� ������Ϊ1
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;
	long pix = 0;
	long piy = 0;
	den->Projection_to_Pixel(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
	//g_CtrlW�Ǵ��ڵ� ��  g_CtrlH�Ǵ��ڵĸ�
	//��� ����� �����ǵĴ��ڵĿ��ӷ�Χ�� �Ż������ ��
	if(pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentFeature->m_visible)  
	//�����Ա������CElement's constructor�г�ʼΪ TRUE�����Ժ���
	{
		//����� m_rendW �� m_rendH ����JS ����WFSPoint��ʱ ���͹���
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
		++discardPoint; //���ڷ�Χ�ڣ������ĵ�
	}
}