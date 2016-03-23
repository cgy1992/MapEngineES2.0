#pragma once
#include "elementlayer.h"
class CGifLayer :
	public CElementLayer
{
public:
	CGifLayer(void);
	~CGifLayer(void);

	int AddElement(CElement* elementObject);
	double GetMinDelay();
	void Draw();
	void Draw(int move);
	void DeleteElement(int ElementHandle);
	bool isEmpty();								//�Ƿ����Ԫ��
	void DeleteTheAllItem();					//ɾ���ڲ���������
	CElement* GetGifElementByHandle(int ElementHandle);		//�ҵ�ELement
private:
	int m_minDelay;
	long m_ElementID_;							//��¼���õ�Ԫ�صľ��
	std::map<long, CElement*> m_ElementMap_;	//GIF����
};

