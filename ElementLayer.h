//element层作用：层可以包含多种要素（element），比如线，面，圆及以后的扩展
//               层提供接口对要素进行增加，删除，隐藏等等操作，在Draw中进行要素的遍历并分别进行渲染
#pragma once
//element层作用：层可以包含多种要素（element），比如线，面，圆及以后的扩展
//               层提供接口对要素进行增加，删除，隐藏等等操作，在Draw中进行要素的遍历并分别进行渲染
#pragma once
#include "layer.h"
#include "Element.h"
#include <vector>
using namespace std;
class CElementLayer :public CLayer
{
public:
	CElementLayer(DrawEngine *den=NULL);
	~CElementLayer(void);

	vector<CElement *> m_ElementList;  //数据集
	int AddElement(CElement* elementObject);//添加要素
	CElement* GetElement(int index);//获取要素
	int GetCount();//获取要素个数
	void DeleteAllElement();//删除所有要素

	void Draw();//渲染：内部是遍历各个要素，并分别调用要素的renderuseogles函数

	//为了快速显示路口
	void DrawAllLineElement();//如果该要素层是路况层，则提供这个接口进行批量绘制道路以提高速度
	GLuint m_vboID;
	DrawEngine *den;
};
