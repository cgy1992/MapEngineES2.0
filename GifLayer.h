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
	bool isEmpty();								//是否存在元素
	void DeleteTheAllItem();					//删除内部所有内容
	CElement* GetGifElementByHandle(int ElementHandle);		//找到ELement
private:
	int m_minDelay;
	long m_ElementID_;							//记录可用的元素的句柄
	std::map<long, CElement*> m_ElementMap_;	//GIF管理
};

