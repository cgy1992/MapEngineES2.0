#include "WFSPolyFeature.h"
#include "LineElement.h"
#include <stdio.h>
/*******************************************CWFSPolyFeature���Ա���������ʵ��***************************************************/
// 1.��poly��[MapEngineES_Point] cast to [CNode]
void CWFSPolyFeature::InitPolyVertex(DrawEngine *den)
{
	//��ʼ����������
	mpNodeList=new CObList(sizeof(CNode));
	//ASSERT(mpNodeList->GetCount());
	//printf("%d\n",mpNodeList->GetCount());
	//int count = mpNodeList->GetCount(); //���������ĳ�ʼֵ count=0�ġ�
	mpTriList=new CObList(sizeof(CTriange));
	mpEdgeList=new CObList(sizeof(CEdge));  //��������еı� ���㷨�Ĵ��������
	
	mpCandidateNode=new CObList(sizeof(CNode));
	mpCandidateNode2=new CObList(sizeof(CNode));
	waihuan = 0;
	currentedge = 0;

	std::vector<MapEngineES_Point>::iterator thePointBegin = m_PointList_.begin();
	std::vector<MapEngineES_Point>::iterator thePointEnd = m_PointList_.end();
	std::vector<MapEngineES_Point>::iterator iter1 = --thePointEnd;  //ָ�����һ���ڵ�
	std::vector<MapEngineES_Point>::iterator iter2 = thePointBegin;
	//for(;thePointBegin != (thePointEnd-1);++thePointBegin)  //�ٱ���һ����
	for(;iter1 > iter2;--iter1)  //��Ϊ�����Poly������� ���á�˳ʱ�롱�ڲ��㣬˳��һ
	{
		CNode temp,beginnode,endnode,temp2;
		CEdge edge; //���� ��Ҫ ��ӽ� �߼��ϵ� edge
		long pix,piy;  //�õ����������
		den->Projection_to_Pixel(iter1->x,iter1->y,pix,piy);
		if (true==wai)
		{
			waihuan++;
			temp.index=waihuan;
			temp.x=pix;
			temp.y=piy;
			temp.out=TRUE;
			if (1==waihuan)
			{
				temp.head=TRUE;
			}			
			//֮ǰ�Ķ���� ���㼯�� �ǿ� ����Ҫ �����µ� edge
			if (!mpNodeList->IsEmpty())
			{
				endnode=*(CNode*)mpNodeList->GetTail();
				++currentedge; //��¼��ǰ ����� edge����������ʼֵ
				edge.index=currentedge;
				edge.L1=endnode.index;
				edge.L2=temp.index;
				mpEdgeList->AddTail(new CEdge(edge)); //��ӵ��߽��
				if((iter1-1) == iter2) //���һ������ �������edge
				{
				  ++currentedge;
				  endnode = *(CNode*)mpNodeList->GetHead();
				  edge.index = currentedge;
				  edge.L1 = temp.index;
				  edge.L2 = endnode.index;
				  mpEdgeList->AddTail(new CEdge(edge));  //����β����  poly�����һ����
				}
			}
		}
		else {; /*Ԥ��������ܻᴦ�� �ڻ�*/ }
		if((iter1-1) == iter2)  temp.tail = true;  //����� ���ᱻ��ӵ� mpNodeList��
		//��mpNodeList���������һ���ظ��� Point
		/*if((thePointEnd-1) != (thePointBegin+1))*/
		mpNodeList->AddTail(new CNode(temp));
	}//��������ÿһ����  forѭ������
	return;
}
//2. Delaunay() �����ʷ�
void CWFSPolyFeature::OnDelaunay()
{
	N=currentedge; //��¼�˶���εı���
	fun1();
	return ;
}

void CWFSPolyFeature::fun1()
{
		//���⣺����ҵ������� [������]
	if (3 == N)    //N=currentnode plygon���ܹ��Ķ�����
	{
		POSITION pos;
		CEdge *tmp;
		int i = 0;
		memset(last,0,sizeof(last));
		//��vertex = 3 ʱ���˼�����Ҳ��ֻʣ���� ����������
		for(pos = mpEdgeList->GetHeadPosition(); pos != NULL;) //ȡ���ı�,��Ϊ������ϻ���ִ�й�����remove
		{
			tmp = (CEdge *)mpEdgeList->GetNext(pos); //�Ὣpos������һ��λ��
			CNode *be,*ed;
			CEdge2 line;
			findnode(tmp,be,ed); //�ҵ�����ֱ��tmp ��[��㣬�յ�]
			line.x1 = be->x;
			line.y1 = be->y;
			line.x2 = ed->x;
			line.y2 = ed->y;
			last[i++] = line;
		}

		//�����һ�� �����εĶ��� ������Ϣ�洢��last��
		for(int j=0;j<3;j++)
		{
			apt2[j].x = last[j].x1;
			apt2[j].y = last[j].y1;
		}
		return;
	}
	else
	{
		k=1;
		mpCandidateNode->RemoveAll();
		mpCandidateNode2->RemoveAll();
		fun2(k);
	}
}
void CWFSPolyFeature::fun2(int &k)
{
	++k;
	if(k>N)
	{
		fun5(k);
		return;
	}
	findedge(1,pL1); //�ҵ���һ�� edge
	findnode(pL1,pL11,pL12); //�ҵ���һ���ߵ� ��ʼ��

	findedge(k,pLk);
	findnode(pLk,pLk1,pLk2);
	//��plygonֻ�����⻷������������ ����һ��edge
	if (((k==N) && pLk2->index==pL11->index)) //modify by xuyan �����k�����ܴ���N
		fun5(k);  
	else if (ToLeft(pL11,pL12,pLk2))
		fun3(k);
	else
		fun2(k);//β�ݹ���ã��൱��whileѭ������indexֱ���ҵ���pL1��ߵ�vertex
}
void CWFSPolyFeature::fun3(int &k)
{
	int t;
	CEdge *pLt;
	CNode *pLt1,*pLt2;

	findedge(1,pL1);
	findnode(pL1,pL11,pL12);
	findedge(k,pLk);
	findnode(pLk,pLk1,pLk2);

	for (t=1;t<=N;t++)    //�ӵ������߿�ʼ��飬��ȥ��1��2��N��k��k+1����
	{
		//k ��ʾ��ǰ�� edge
		if ((t!=k) && (t!=k+1))
		{
			findedge(t,pLt);
			findnode(pLt,pLt1,pLt2);
			//���ж��ཻ���㷨�У���ֱ�����ڵ�����edge�����ཻ
			if (intersect(pLt1,pLt2,pL11,pLk2) || intersect(pLt1,pLt2,pL12,pLk2) )
			{
				//���ཻ
				fun2(k);
				return;
			}
		}
	}
	if (t==N+1)//����  û�б�Ҫ ��������ж�
	{
		fun4(k);
	}
}
void CWFSPolyFeature::fun4(int &k)
{
	findedge(k,pLk);
	findnode(pLk,pLk1,pLk2);
	mpCandidateNode->AddTail(new CNode(*pLk2));
	if (k==N)
	{
		fun5(k);
	}
	else 
		fun2(k);
}
void CWFSPolyFeature::fun5(int &k)
{
	double minR=65535;
	double r=0;
	CEdge *pLn,*pLm;
	CTriange tri;
	CRgn  rgn;
	CPoint ptVertex[3];
	int M_index=0;//
	int N_index=0;//

	POSITION pos;

	findedge(1,pL1); 
	findnode(pL1,pL11,pL12); //��edge pL1��[��,��]vertex
	findLo2(pL11,pL12,pLo2,mpCandidateNode);   //����mpCandidateNode�ǿ�  ��
	CNode *pLcan;
	while (true==IsCanExistInCircle(pL11,pL12,pLo2))				
	{
		mpCandidateNode->RemoveAll();
		for(pos=mpCandidateNode2->GetHeadPosition();pos!=NULL;)
		{
			pLcan=(CNode*)mpCandidateNode->GetNext(pos);
			mpCandidateNode->AddTail(new CNode(*pLcan));
		}
		findLo2(pL11,pL12,pLo2,mpCandidateNode);
	}
	tri.index=++tri_index;
	tri.L1=pL11->index; 
	tri.L2=pL12->index; 
	tri.L3=pLo2->index;   // ���pLo2������ û��ȡ����ȷ��ֵ
	mpTriList->AddTail(new CTriange(tri));  //��Ҫ �ǵõ����������� ����
	
	if (!isEdge(pL11,pLo2,M_index) && !isEdge(pL12,pLo2,N_index))//????L11LO2?L12LO2?????????????
	{
		POSITION pre;
		CEdge* pa;
		++N;
		pLn=new CEdge();
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;)
		{
			pre=pos;
			pa=(CEdge*)mpEdgeList->GetNext(pos);
			if(pa->L2==pL1->L1)	break;
		}
		if(!pre) exit(0);

		for(;pos!=NULL;)
		{
			pa=(CEdge*)mpEdgeList->GetNext(pos);
			pa->index++;
		}
		pa=(CEdge*)mpEdgeList->GetAt(pre);
		pLn->index=pa->index+1;
		pLn->L1=pL11->index;
		pLn->L2=pLo2->index;
		mpEdgeList->InsertAfter(pre,new CEdge(*pLn));
		pL1->L1=pLo2->index;
	}
	else if ( isEdge(pL11,pLo2,M_index) && !isEdge(pL12,pLo2,N_index) )
	{
		CEdge* pa;
		int i=1;

		pos=mpEdgeList->FindIndex(M_index-1);
		mpEdgeList->RemoveAt(pos);  
		pL1->L1=pLo2->index;
		
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;i++)
		{
			pa=(CEdge*)mpEdgeList->GetNext(pos);
			if(pa->index!=i)	pa->index=i;
		}
		--N;
	}
	else if ( isEdge(pL12,pLo2,M_index) && !isEdge(pL11,pLo2,N_index) )
	{
		CEdge* pa;
		int i=1;

		pL1->L2=pLo2->index;
		findedge(M_index,pLm);
		findedge(N,pLn);
		pos=mpEdgeList->FindIndex(M_index-1);
		mpEdgeList->RemoveAt(pos);
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;)//����Ҫ��
		{
			pa=(CEdge*)mpEdgeList->GetNext(pos);
			pa->index=i++;
		}
		--N;
	}
	else if ( isEdge(pL12,pLo2,M_index) && isEdge(pL11,pLo2,N_index) )
	{
		CObject *pa;
		if(M_index>N_index)	{int mid=N_index;N_index=M_index;M_index=N_index;}

		pos=mpEdgeList->FindIndex(N_index-1);
		pa=mpEdgeList->GetAt(pos);
		mpEdgeList->RemoveAt(pos);
		delete pa;
		pos=mpEdgeList->FindIndex(M_index-1);
		pa=mpEdgeList->GetAt(pos);
		mpEdgeList->RemoveAt(pos);
		delete pa;
		pos=mpEdgeList->FindIndex(1-1);
		pa=mpEdgeList->GetAt(pos);
		mpEdgeList->RemoveAt(pos);
		delete pa;
		
		//���¸� �߼��� �е�edge��index���
		int i=1;
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;)
			((CEdge*)mpEdgeList->GetNext(pos))->index=i++;
		N-=3;
	}
	fun1();
}
/********************************************************************************************************************************/


/*****************************************������صļ��㺯��ʵ��****************************************************************/
bool CWFSPolyFeature::IsCanExistInCircle(CNode* pL11,CNode* pL12,CNode* pLo2)
{
//	CPoint ptVertex[3];
	POSITION pos;
	CNode *pLcan;
	CRgn rgn;
	bool exist=FALSE;
	mpCandidateNode2->RemoveAll();
	
	for (pos=mpCandidateNode->GetHeadPosition();pos!=NULL;)
	{
		pLcan=(CNode*)mpCandidateNode->GetNext(pos);
		if (pLcan->index!=pLo2->index)
		{
			if (ToLeft(pL11,pL12,pLcan)&&ToLeft(pL12,pLo2,pLcan)&&ToLeft(pLo2,pL11,pLcan))
			{
				mpCandidateNode2->AddTail(new CNode(*pLcan));
				exist=true;
			}
		}
	}
	if (0==mpCandidateNode2->GetCount())
	{
		exist=false;
	}
	else 
		exist=true;
	return exist;
}
void CWFSPolyFeature::findLo2(CNode* pL11,CNode* pL12,CNode*& pLo2,CObList *pCandidateList)
{
	POSITION pos;
	CNode* pLcan=NULL;
	double minR=65535;
	double r=0;

	for (pos=pCandidateList->GetHeadPosition();pos!=NULL; )
	{
		pLcan=(CNode*)pCandidateList->GetNext(pos);
		findedge(1,pL1);
		findnode(pL1,pL11,pL12);
		r=waixin(pL11,pL12,pLcan);
		if (r<minR)
		{
			minR=r;
			pLo2=pLcan;
		}
	}
}
void CWFSPolyFeature::findedge(int k,CEdge*& pLk)
{
	POSITION pos;
	pos=mpEdgeList->FindIndex(k-1);
	pLk=(CEdge*)mpEdgeList->GetAt(pos);
}

void CWFSPolyFeature::findnode(CEdge* pedge, CNode*& pbeginnode, CNode*& pendnode)
{
	POSITION pos;
	if(pedge->L1 <= currentedge && pedge->L2 <= currentedge)  //modify by xuyan  ��������������崻�
	{
		pos=mpNodeList->FindIndex(pedge->L1-1);
//		assert(pos);  //add by xuyan  ��release version�в�����
		pbeginnode=(CNode*)mpNodeList->GetAt(pos);
		pos=mpNodeList->FindIndex(pedge->L2-1);
		pendnode=(CNode*)mpNodeList->GetAt(pos);
	}

}
bool CWFSPolyFeature::isEdge(CNode* pL1,CNode* L2,int &M)
{
	POSITION pos;
	CEdge edge;
	for (pos=mpEdgeList->GetHeadPosition();pos!=NULL;)
	{
		edge=*(CEdge*)mpEdgeList->GetNext(pos);
		if ((edge.L1==pL1->index) && (edge.L2==L2->index) ||(edge.L1==L2->index) && (edge.L2==pL1->index))
		{
			M=edge.index;
			return TRUE;
		}
	}
	return false;
}
double CWFSPolyFeature::waixin(CNode* p1,CNode* p2,CNode* p3)                    //�����Բ�뾶
{
	double a,b,c,s;
	a=sqrt((double)((p2->y-p1->y)*(p2->y-p1->y)+(p2->x-p1->x)*(p2->x-p1->x)));
	b=sqrt((double)((p3->y-p2->y)*(p3->y-p2->y)+(p3->x-p2->x)*(p3->x-p2->x)));
	c=sqrt((double)((p1->y-p3->y)*(p1->y-p3->y)+(p1->x-p3->x)*(p1->x-p3->x)));

	s=(a+b+c)/2;
	s=sqrt(s*(s-a)*(s-b)*(s-c));
	circler=a*b*c/(4*s);

	return circler;
}
int CWFSPolyFeature::Area2(CNode* pa,CNode* pb,CNode* pc)
{
	double r=(
		pa->x * pb->y - pa->y * pb->x
		+	pb->x * pc->y - pb->y * pc->x
		+	pc->x * pa->y - pc->y * pa->x);
	if (r>0)
	{
		return 1;
	}
	else if (0==r)
	{
		return 0;
	}
	else 
	{
		return -1;
	}

}
bool CWFSPolyFeature::ToLeft(CNode* pa,CNode*	pb,CNode* px)
{
	int r=Area2(pa, pb, px);
	return(r<0);
}
int CWFSPolyFeature::WhichSide(CNode* pp,CNode* pp1,CNode* pp2)   
{   
	double   equation;   
	equation   =   ((pp1->y   -   pp2->y)   *   (pp->x   -   pp2->x))   -   ((pp->y   -   pp2->y)   *   (pp1->x   -   pp2->x));   
	if   (equation>0)   return   1;   
	else if(equation==0)	return   0;   
    else	return   -1;
}
bool CWFSPolyFeature::intersect(CNode* pP1,CNode* pP2,CNode* pP3,CNode* pP4)         //�߶���
{
	
	if(pP1->index==pP3->index||pP2->index==pP3->index||pP1->index==pP4->index||pP2->index==pP4->index)
		return FALSE;
	else
	{
		if((Area2(pP1,pP2,pP3)*Area2(pP1,pP2,pP4)<=0) && (Area2(pP3,pP4,pP1)*Area2(pP3,pP4,pP2)<=0))	
			return TRUE;
		else 
			return FALSE;
	}

}
bool CWFSPolyFeature::samep(CNode* pp1,CNode* pp2)
{
    if ((pp1->x==pp2->x)&&(pp1->y==pp2->y))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
double CWFSPolyFeature::dist(CNode& pp1,CNode& pp2)
{	
    double ret=0;
    ret=sqrt( double((pp1.x-pp2.x)*(p1.x-p2.x)+ (p1.y-p2.y)*(p1.y-p2.y)));
    return ret;
}
CNode CWFSPolyFeature::midpoint(CNode* pp1, CNode* pp2)
{
    CNode ret;
    ret.x = (pp1->x + pp2->x )/2;
    ret.y = (pp1->y + pp2->y )/2;
    return ret;
}
CLine CWFSPolyFeature::line2p(CNode* pp1,CNode* pp2)//û����ˮƽ�봹ֱ
{
    CLine ret;
    if ((pp1->x==pp2->x) && (pp1->y==pp2->y))
    {
        ret.k = 0;
        ret.b = 0;
    }
    else
    {
        ret.k = (pp1->y - pp2->y)/ (pp1->x -pp2->x);
        ret.b = pp1->y - pp1->x * ret.k ;
    }
    return ret;
}
bool CWFSPolyFeature::iscross(CLine l1,CLine l2)
{
    if (l1.k==l2.k)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
bool CWFSPolyFeature::isatline(CLine l1,CNode* pp)
{
	if (pp->y == l1.k*pp->x+l1.b)
		return true;
	return false;
}
CNode CWFSPolyFeature::crosspoint(CLine l1,CLine l2)
{
    CNode ret;
    if (!iscross(l1,l2))
    {
        ret.x = 0;
        ret.y = 0;
    }
    else
    {
        ret.x = (l2.b-l1.b ) / (l1.k - l2.k);
        ret.y = (l1.k*l2.b-l2.k*l1.b)/(l1.k-l2.k);
    }
    return ret;
}
CLine CWFSPolyFeature::linepoint(CNode p,double k,double n) //��ĳ��ֱ�߷���
{
    CLine ret;
    ret.k = tan(atan(k)+n);
    ret.b = p.y -  ret.k * p.x;
    return ret;
}
/***********************************************************************************************************************/

//[ʵ�����εĻ��Ʒ���] �Ȼ��Ʊ߿��ٻ���������Ƭ���
void CWFSPolyFeatureSet::RenderManyUseOgles(int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den)
{
	// scane all the Poly
	std::map<long, CWFSPolyFeature*>::iterator thePolyBegin = m_DataSet_.begin();
	std::map<long, CWFSPolyFeature*>::iterator thePolyEnd = m_DataSet_.end();
	//�ĸ����� ��������������
	//���ʹ��long ����ʾ���꣬��������صľ��ȶ�ʧ�����º�����Ƶ�����ƫ���Լ�������������λ�ò�׼
	float x1 = 0.0f;
	float y1 = 0.0f;
	float x2 = 0.0f;
	float y2 = 0.0f;
	float x3 = 0.0f;
	float y3 = 0.0f;
	float x4 = 0.0f;
	float y4 = 0.0f;
	//��ߵ����궼�Ѿ� ����������
	std::vector<Vertex> thePolyData; //���������� vertex�ĸ���
	std::vector<Vertex> pointVec; //���ڲ��յ�
	std::vector<Vertex> directVec; //���ڱ�ʾ ·�εķ���
	Vertex theCurrentData;
	theCurrentData.tu = 0;
	theCurrentData.tv = 0;
	theCurrentData.color[0] = rcolor / 255.0;
	theCurrentData.color[1] = gcolor / 255.0;
	theCurrentData.color[2] = bcolor / 255.0;
	theCurrentData.color[3] = acolor / 255.0;

	CLineElement thePolyElement(den);  //��Line�Ļ�����,��Ϊ�����ṩ���������������εķ���������Ҳ����������[ʵ������]

	int pWidth = width;  //��js��cmd���� �����ֵ��100  ��ʾ Poly�Ŀ��
	double d;
	////if (m_unit.Compare(L"metre") == 0 )  //�����Ĭ��ֵ ��"" ֻ��·�� ����metre  PolyLuKuang->m_unit = L"metre"
	////{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		//pWidth = pWidth*(96*39.7)/scale;//��Ҫ��֤????
		double an = pWidth / (2 * 3.1415926 * 6378137.0) * 360;
		//d = w / (2 * 3.1415926 * 6378137.0) * 360;
	     pWidth = an/resolution;
	//}
	thePolyElement.m_w = pWidth; //���� ��ʾ·���� �߿� �����Ѿ�ת��Ϊ ��pixelΪ��λ
	thePolyElement.m_type = 1;// ����type��ʾ ����invoke  DrawPolys()
	
	while(thePolyBegin != thePolyEnd) //�������е�Poly��(һ��Poly�����ɶ����ɢ��Point���)
	{
		
		// scane all the point
		std::vector<MapEngineES_Point>::iterator thePointBegin = thePolyBegin->second->m_PointList_.begin();
		std::vector<MapEngineES_Point>::iterator thePointEnd = thePolyBegin->second->m_PointList_.end();
		std::vector<MapEngineES_Point>::iterator thePointPre;
		while(thePointBegin != thePointEnd)  //ѭ���ڲ� һ���Դ��� �������ڵ� ��
		{
			thePointPre = thePointBegin;		// ��һ��
			long prexpix = 0;
			long preypiy = 0;
			den->Projection_to_Pixel(thePointPre->x, thePointPre->y, prexpix, preypiy);  //�������� ���������ԭ�򣬻��Ƶ�Line�� ���ͼ�ƶ����ƶ�
/**************add by xuyan  ���ڲ��յ�*******************************/
			theCurrentData.loc[0] = prexpix;
			theCurrentData.loc[1] = preypiy;
			theCurrentData.loc[2] = 1;
			pointVec.push_back(theCurrentData);   //�����������·����β��Ҳ�Ჹ��
/****************һ��ѭ������������****************************************/
			thePointBegin++;
			//���� ÿһ��·�� �յ�
			if(thePointEnd == thePointBegin || thePointPre->fid != thePointBegin->fid)
			{
				theCurrentData.loc[0] = prexpix;
				theCurrentData.loc[1] = preypiy;
				theCurrentData.loc[2] = 1;
				directVec.push_back(theCurrentData);
			}
			if(thePointBegin != thePointEnd)
			{
				// construct the triangle
				long curxpix = 0;
				long curypiy = 0;
				den->Projection_to_Pixel(thePointBegin->x, thePointBegin->y, curxpix, curypiy);

				if (prexpix == curxpix)
				{
					x1 = prexpix-pWidth/2;
					x2 = prexpix+pWidth/2;
					x3 = curxpix-pWidth/2;
					x4 = curxpix+pWidth/2;
					y1 = preypiy;
					y2 = preypiy;
					y3 = curypiy;
					y4 = curypiy;
				}
				else if (preypiy == curypiy)
				{
					x1 = prexpix;
					x2 = prexpix;
					x3 = curxpix;
					x4 = curxpix;
					y1 = preypiy+pWidth/2;
					y2 = preypiy-pWidth/2;
					y3 = curypiy+pWidth/2;
					y4 = curypiy-pWidth/2;
				}
				else  //���ڵ����� �� ֮���λ�� ��������λ��
				{
					double d=(curypiy-preypiy)*1.0/(prexpix-curxpix);
					y1=pWidth/2.0/sqrt(d*d+1.0)+preypiy;
					x1=d*(y1-preypiy) +prexpix;
					y2=-pWidth/2.0/sqrt(d*d+1.0)+preypiy;
					x2=d*(y2-preypiy)+prexpix;
					y3=pWidth/2.0/sqrt(d*d+1.0)+curypiy;
					x3=d*(y3-curypiy)+curxpix;
					y4=-pWidth/2.0/sqrt(d*d+1.0)+curypiy;
					x4=d*(y4-curypiy)+curxpix;
				}
				//���ڵ��������� ֮��Ŀ��߶� ������һ��ʵ����Σ�����ʹ������ ����������ʾ�������

				//1 2 3
				theCurrentData.loc[0] = x1;
				theCurrentData.loc[1] = y1;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				theCurrentData.loc[0] = x2;
				theCurrentData.loc[1] = y2;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				theCurrentData.loc[0] = x3;
				theCurrentData.loc[1] = y3;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				//2 3 4
				theCurrentData.loc[0] = x3;
				theCurrentData.loc[1] = y3;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				theCurrentData.loc[0] = x2;
				theCurrentData.loc[1] = y2;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				theCurrentData.loc[0] = x4;
				theCurrentData.loc[1] = y4;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);
			}
			
		}
		thePolyBegin++;  //���� ѭ��������������ʾ������һ������
	}
	//��ԭ�еĽӿ�����Ӳ��� ������Ĭ�ϲ���������,�����˶�ԭ���ӿڵļ����ԡ�thePolyData���Ǳ����� pixelΪ��λ��������Ϣ
	thePolyElement.RenderManyOgles(thePolyData,pointVec/*,directVec*/);//���� ��ʾ������,�ڶ����������ڲ��յ㣬����������ֻ�ǲ���ʹ�ò�����ʾ·�η���

	thePolyData.clear();// �������,������fill����ε����������Ƭʹ�� type:vector<Vertex>
  //��������� ��� �����������Ĵ���
	thePolyBegin = m_DataSet_.begin(); //���±������е�Poly
	for(;thePolyBegin != thePolyEnd;++thePolyBegin)   
	{
		//if(thePolyBegin->first == 1 || thePolyBegin->first == 3)  
		//{
			//1.��poly�Ķ������� ��Ԥ��������CNode��������
			thePolyBegin->second->InitPolyVertex(den);
			//2.���������ݣ�������������Ƭ�Ķ�������
			thePolyBegin->second->OnDelaunay();   //���������һ�ſ��ͻᵼ�� �ҵ�
			//3.������Ƭ��vertex���ݣ����������� thePolyData
			//Poly ���е�������Ƭ���������һ��
			POSITION pos;
			for(pos = thePolyBegin->second->mpTriList->GetHeadPosition();pos != NULL ;) 
			{
				CTriange Triange = *(CTriange *)thePolyBegin->second->mpTriList->GetNext(pos);
				POINT apt[3]; //�����ε�vertex

				POSITION pos;
				pos=thePolyBegin->second->mpNodeList->FindIndex(Triange.L1 - 1);
				CNode *pTemp =(CNode *)thePolyBegin->second->mpNodeList->GetAt(pos);
				theCurrentData.loc[0] = pTemp->x;
				theCurrentData.loc[1] = pTemp->y;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				pos=thePolyBegin->second->mpNodeList->FindIndex(Triange.L2 - 1);
				pTemp =(CNode *)thePolyBegin->second->mpNodeList->GetAt(pos);
				theCurrentData.loc[0] = pTemp->x;
				theCurrentData.loc[1] = pTemp->y;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);

				pos=thePolyBegin->second->mpNodeList->FindIndex(Triange.L3 - 1);
				pTemp =(CNode *)thePolyBegin->second->mpNodeList->GetAt(pos);
				theCurrentData.loc[0] = pTemp->x;
				theCurrentData.loc[1] = pTemp->y;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);
			}
			for(int i=0;i<3;i++)  //���һ�������Σ��ڵݹ�ĳ��ڳ���ʹ��ȫ�ֱ���apt2����
			{
				theCurrentData.loc[0] = thePolyBegin->second->apt2[i].x;
				theCurrentData.loc[1] = thePolyBegin->second->apt2[i].y;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);
			}
		//}//���� if ���� ˳ʱ�빹��Ķ����
	   
	} //���forѭ�� �������е�Poly
	//4.�����������
	thePolyElement.RenderManyOgles(thePolyData,std::vector<Vertex>()); //ֻ��������Ƭ���������[������]
}


void CWFSPolyFeatureSet::RemoveAllFeature()
{
	std::map<long, CWFSPolyFeature*>::iterator theBegin = m_DataSet_.begin();
	std::map<long, CWFSPolyFeature*>::iterator theEnd = m_DataSet_.end();
	while (theBegin != theEnd)
	{
		if(theBegin->second)
			delete theBegin->second;
		theBegin++;
	}
	m_DataSet_.clear();
}