#include "WFSPolyFeature.h"
#include "LineElement.h"
#include <stdio.h>
/*******************************************CWFSPolyFeature类成员函数的相关实现***************************************************/
// 1.将poly的[MapEngineES_Point] cast to [CNode]
void CWFSPolyFeature::InitPolyVertex(DrawEngine *den)
{
	//初始化辅助容器
	mpNodeList=new CObList(sizeof(CNode));
	//ASSERT(mpNodeList->GetCount());
	//printf("%d\n",mpNodeList->GetCount());
	//int count = mpNodeList->GetCount(); //这里容器的初始值 count=0的。
	mpTriList=new CObList(sizeof(CTriange));
	mpEdgeList=new CObList(sizeof(CEdge));  //这个容器中的边 在算法的处理过程中
	
	mpCandidateNode=new CObList(sizeof(CNode));
	mpCandidateNode2=new CObList(sizeof(CNode));
	waihuan = 0;
	currentedge = 0;

	std::vector<MapEngineES_Point>::iterator thePointBegin = m_PointList_.begin();
	std::vector<MapEngineES_Point>::iterator thePointEnd = m_PointList_.end();
	std::vector<MapEngineES_Point>::iterator iter1 = --thePointEnd;  //指向最后一个节点
	std::vector<MapEngineES_Point>::iterator iter2 = thePointBegin;
	//for(;thePointBegin != (thePointEnd-1);++thePointBegin)  //少遍历一个点
	for(;iter1 > iter2;--iter1)  //因为这里的Poly的外框是 采用“顺时针”在布点，顺序不一
	{
		CNode temp,beginnode,endnode,temp2;
		CEdge edge; //构造 将要 添加进 边集合的 edge
		long pix,piy;  //该点的像素坐标
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
			//之前的多边形 顶点集合 非空 ，需要 构造新的 edge
			if (!mpNodeList->IsEmpty())
			{
				endnode=*(CNode*)mpNodeList->GetTail();
				++currentedge; //记录当前 多边形 edge的条数，初始值
				edge.index=currentedge;
				edge.L1=endnode.index;
				edge.L2=temp.index;
				mpEdgeList->AddTail(new CEdge(edge)); //添加到边结合
				if((iter1-1) == iter2) //最后一个定点 添加两条edge
				{
				  ++currentedge;
				  endnode = *(CNode*)mpNodeList->GetHead();
				  edge.index = currentedge;
				  edge.L1 = temp.index;
				  edge.L2 = endnode.index;
				  mpEdgeList->AddTail(new CEdge(edge));  //将首尾相连  poly的最后一条边
				}
			}
		}
		else {; /*预留这里可能会处理 内环*/ }
		if((iter1-1) == iter2)  temp.tail = true;  //这个点 不会被添加到 mpNodeList中
		//在mpNodeList不保存最后一个重复的 Point
		/*if((thePointEnd-1) != (thePointBegin+1))*/
		mpNodeList->AddTail(new CNode(temp));
	}//迭代处理每一个点  for循环结束
	return;
}
//2. Delaunay() 三角剖分
void CWFSPolyFeature::OnDelaunay()
{
	N=currentedge; //记录了多边形的边数
	fun1();
	return ;
}

void CWFSPolyFeature::fun1()
{
		//问题：如何找到这最后的 [三条边]
	if (3 == N)    //N=currentnode plygon的总共的顶点数
	{
		POSITION pos;
		CEdge *tmp;
		int i = 0;
		memset(last,0,sizeof(last));
		//当vertex = 3 时，此集合中也就只剩下了 最后的三条边
		for(pos = mpEdgeList->GetHeadPosition(); pos != NULL;) //取最后的边,因为这个集合会在执行过程中remove
		{
			tmp = (CEdge *)mpEdgeList->GetNext(pos); //会将pos推向下一个位置
			CNode *be,*ed;
			CEdge2 line;
			findnode(tmp,be,ed); //找到属于直线tmp 的[起点，终点]
			line.x1 = be->x;
			line.y1 = be->y;
			line.x2 = ed->x;
			line.y2 = ed->y;
			last[i++] = line;
		}

		//将最后一个 三角形的顶点 坐标信息存储在last中
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
	findedge(1,pL1); //找到第一条 edge
	findnode(pL1,pL11,pL12); //找到第一条边的 起始点

	findedge(k,pLk);
	findnode(pLk,pLk1,pLk2);
	//该plygon只存在外环，并且现在是 最后的一条edge
	if (((k==N) && pLk2->index==pL11->index)) //modify by xuyan 这里的k不可能大于N
		fun5(k);  
	else if (ToLeft(pL11,pL12,pLk2))
		fun3(k);
	else
		fun2(k);//尾递归调用，相当于while循环，按index直到找到在pL1左边的vertex
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

	for (t=1;t<=N;t++)    //从第三条边开始检查，除去第1，2，N，k，k+1条边
	{
		//k 表示当前的 edge
		if ((t!=k) && (t!=k+1))
		{
			findedge(t,pLt);
			findnode(pLt,pLt1,pLt2);
			//在判断相交的算法中，与直线相邻的两个edge不算相交
			if (intersect(pLt1,pLt2,pL11,pLk2) || intersect(pLt1,pLt2,pL12,pLk2) )
			{
				//若相交
				fun2(k);
				return;
			}
		}
	}
	if (t==N+1)//改了  没有必要 加入这个判断
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
	findnode(pL1,pL11,pL12); //找edge pL1的[起,终]vertex
	findLo2(pL11,pL12,pLo2,mpCandidateNode);   //这里mpCandidateNode是空  ，
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
	tri.L3=pLo2->index;   // 这个pLo2有问题 没有取到正确的值
	mpTriList->AddTail(new CTriange(tri));  //主要 是得到这个容器里的 数据
	
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
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;)//不需要吧
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
		
		//重新给 边集合 中的edge的index编号
		int i=1;
		for(pos=mpEdgeList->GetHeadPosition();pos!=NULL;)
			((CEdge*)mpEdgeList->GetNext(pos))->index=i++;
		N-=3;
	}
	fun1();
}
/********************************************************************************************************************************/


/*****************************************几何相关的计算函数实现****************************************************************/
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
	if(pedge->L1 <= currentedge && pedge->L2 <= currentedge)  //modify by xuyan  就是这里引发的宕机
	{
		pos=mpNodeList->FindIndex(pedge->L1-1);
//		assert(pos);  //add by xuyan  在release version中不可用
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
double CWFSPolyFeature::waixin(CNode* p1,CNode* p2,CNode* p3)                    //求外接圆半径
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
bool CWFSPolyFeature::intersect(CNode* pP1,CNode* pP2,CNode* pP3,CNode* pP4)         //线段求交
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
CLine CWFSPolyFeature::line2p(CNode* pp1,CNode* pp2)//没考虑水平与垂直
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
CLine CWFSPolyFeature::linepoint(CNode p,double k,double n) //过某点直线方程
{
    CLine ret;
    ret.k = tan(atan(k)+n);
    ret.b = p.y -  ret.k * p.x;
    return ret;
}
/***********************************************************************************************************************/

//[实体多边形的绘制方法] 先绘制边框，再绘制三角面片填充
void CWFSPolyFeatureSet::RenderManyUseOgles(int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den)
{
	// scane all the Poly
	std::map<long, CWFSPolyFeature*>::iterator thePolyBegin = m_DataSet_.begin();
	std::map<long, CWFSPolyFeature*>::iterator thePolyEnd = m_DataSet_.end();
	//四个顶点 构造两个三角形
	//如果使用long 来表示坐标，会造成严重的精度丢失，导致后面绘制的线条偏移以及补点与线条的位置不准
	float x1 = 0.0f;
	float y1 = 0.0f;
	float x2 = 0.0f;
	float y2 = 0.0f;
	float x3 = 0.0f;
	float y3 = 0.0f;
	float x4 = 0.0f;
	float y4 = 0.0f;
	//里边的坐标都已经 是像素坐标
	std::vector<Vertex> thePolyData; //构造三角形 vertex的个数
	std::vector<Vertex> pointVec; //用于补拐点
	std::vector<Vertex> directVec; //用于表示 路段的方向
	Vertex theCurrentData;
	theCurrentData.tu = 0;
	theCurrentData.tv = 0;
	theCurrentData.color[0] = rcolor / 255.0;
	theCurrentData.color[1] = gcolor / 255.0;
	theCurrentData.color[2] = bcolor / 255.0;
	theCurrentData.color[3] = acolor / 255.0;

	CLineElement thePolyElement(den);  //绘Line的基础类,因为里面提供了批量绘制三角形的方法，所以也被用来绘制[实体多边形]

	int pWidth = width;  //由js的cmd发来 这里的值是100  表示 Poly的宽度
	double d;
	////if (m_unit.Compare(L"metre") == 0 )  //这里的默认值 是"" 只有路况 才是metre  PolyLuKuang->m_unit = L"metre"
	////{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		//pWidth = pWidth*(96*39.7)/scale;//需要验证????
		double an = pWidth / (2 * 3.1415926 * 6378137.0) * 360;
		//d = w / (2 * 3.1415926 * 6378137.0) * 360;
	     pWidth = an/resolution;
	//}
	thePolyElement.m_w = pWidth; //设置 显示路况的 线宽 这里已经转换为 以pixel为单位
	thePolyElement.m_type = 1;// 这里type表示 将会invoke  DrawPolys()
	
	while(thePolyBegin != thePolyEnd) //遍历所有的Poly，(一条Poly又是由多个离散的Point组成)
	{
		
		// scane all the point
		std::vector<MapEngineES_Point>::iterator thePointBegin = thePolyBegin->second->m_PointList_.begin();
		std::vector<MapEngineES_Point>::iterator thePointEnd = thePolyBegin->second->m_PointList_.end();
		std::vector<MapEngineES_Point>::iterator thePointPre;
		while(thePointBegin != thePointEnd)  //循环内部 一次性处理 两个相邻的 点
		{
			thePointPre = thePointBegin;		// 下一个
			long prexpix = 0;
			long preypiy = 0;
			den->Projection_to_Pixel(thePointPre->x, thePointPre->y, prexpix, preypiy);  //正是由于 这个函数的原因，绘制的Line会 随地图移动而移动
/**************add by xuyan  用于补拐点*******************************/
			theCurrentData.loc[0] = prexpix;
			theCurrentData.loc[1] = preypiy;
			theCurrentData.loc[2] = 1;
			pointVec.push_back(theCurrentData);   //放置在这里后，路况的尾端也会补点
/****************一次循环处理两个点****************************************/
			thePointBegin++;
			//保存 每一条路的 终点
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
				else  //相邻的两个 点 之间的位置 不是特殊位置
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
				//相邻的两个顶点 之间的宽线段 构成了一个实体矩形，我们使用两个 三角形来表示这个矩形

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
		thePolyBegin++;  //外层的 循环记数变量，表示处理完一条线了
	}
	//在原有的接口上添加参数 均采用默认参数的做法,保持了对原来接口的兼容性。thePolyData就是保存了 pixel为单位的坐标信息
	thePolyElement.RenderManyOgles(thePolyData,pointVec/*,directVec*/);//批量 显示三角形,第二个参数用于补拐点，第三个参数只是测试使用补大点表示路段方向

	thePolyData.clear();// 清空容器,供下面fill多边形的区域的三角片使用 type:vector<Vertex>
  //在下面继续 添加 填充多边形区域的代码
	thePolyBegin = m_DataSet_.begin(); //重新遍历所有的Poly
	for(;thePolyBegin != thePolyEnd;++thePolyBegin)   
	{
		//if(thePolyBegin->first == 1 || thePolyBegin->first == 3)  
		//{
			//1.将poly的顶点数据 做预处理保存在CNode的容器中
			thePolyBegin->second->InitPolyVertex(den);
			//2.处理顶点数据，并将最后的三角片的顶点数据
			thePolyBegin->second->OnDelaunay();   //把这个函数一放开就会导致 挂掉
			//3.将三角片的vertex数据，保存在容器 thePolyData
			//Poly 所有的三角面片，除了最后一个
			POSITION pos;
			for(pos = thePolyBegin->second->mpTriList->GetHeadPosition();pos != NULL ;) 
			{
				CTriange Triange = *(CTriange *)thePolyBegin->second->mpTriList->GetNext(pos);
				POINT apt[3]; //三角形的vertex

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
			for(int i=0;i<3;i++)  //最后一个三角形，在递归的出口出，使用全局变量apt2保存
			{
				theCurrentData.loc[0] = thePolyBegin->second->apt2[i].x;
				theCurrentData.loc[1] = thePolyBegin->second->apt2[i].y;
				theCurrentData.loc[2] = 1;
				thePolyData.push_back(theCurrentData);
			}
		//}//测试 if 保护 顺时针构造的多边形
	   
	} //外层for循环 处理所有的Poly
	//4.绘制填充区域
	thePolyElement.RenderManyOgles(thePolyData,std::vector<Vertex>()); //只绘制三角片，补点的是[空容器]
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