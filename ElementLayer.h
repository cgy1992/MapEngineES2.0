//element�����ã�����԰�������Ҫ�أ�element���������ߣ��棬Բ���Ժ����չ
//               ���ṩ�ӿڶ�Ҫ�ؽ������ӣ�ɾ�������صȵȲ�������Draw�н���Ҫ�صı������ֱ������Ⱦ
#pragma once
//element�����ã�����԰�������Ҫ�أ�element���������ߣ��棬Բ���Ժ����չ
//               ���ṩ�ӿڶ�Ҫ�ؽ������ӣ�ɾ�������صȵȲ�������Draw�н���Ҫ�صı������ֱ������Ⱦ
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

	vector<CElement *> m_ElementList;  //���ݼ�
	int AddElement(CElement* elementObject);//���Ҫ��
	CElement* GetElement(int index);//��ȡҪ��
	int GetCount();//��ȡҪ�ظ���
	void DeleteAllElement();//ɾ������Ҫ��

	void Draw();//��Ⱦ���ڲ��Ǳ�������Ҫ�أ����ֱ����Ҫ�ص�renderuseogles����

	//Ϊ�˿�����ʾ·��
	void DrawAllLineElement();//�����Ҫ�ز���·���㣬���ṩ����ӿڽ����������Ƶ�·������ٶ�
	GLuint m_vboID;
	DrawEngine *den;
};
