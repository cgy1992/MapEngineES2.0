#pragma once
#include "Layer.h"
#include <set>
//#include "CELLShader.hpp"


/*
 ���ͼ��
*/
class DrawEngine;
class CMarkerElement;

class PlottingLayer :
	public CLayer
{
	typedef int StyleId;
	typedef unsigned LabelId;
public:
	PlottingLayer(DrawEngine *den, VLayerType layerType = vPlottingLayer);
	~PlottingLayer(){ delete _rendMark; _rendMark = NULL; }
public: //�ṩ��API�ӿ�
	LabelId addMark(const MarkPoint &pt); //���ӱ�棬����ͼ���ID ���ڹ���
	void deleteMarkById(LabelId lableId);
	void modifyMarkPosById(LabelId lableId, const GeoPos &pos);//�޸� ��label��λ��
	void clearMark(); //������еı��
	void synMark(); //��ǽ�� ͬ�����е� ͼ��
	virtual void Draw();
private:
	//PROGRAM_POINT _shader;
	CMarkerElement *_rendMark;
	DrawEngine *_den;
	VLayerType _layerType; //ͼ������
	LabelId _labelId;  //��ʶ mark��ΨһID����ʼ��
	std::set<StyleId> _styleSet;
	std::map<LabelId, MarkPoint> _markData; //�������
};

