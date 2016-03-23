#pragma once
#include "stdafx.h"
#include "DataStruct.h"


class CLayer
{
public:
	CLayer();
	virtual ~CLayer(void);
	virtual void Draw(){};

	double m_x;//���ĵ�
	double m_y;
	int m_js; //��ǰ��ʾ����
	int m_PicW;//��Ƭ�����һ�µ�
	int m_PicH;

	CString m_MapName;//��ͼ������ͼ����
	CString m_sPath;//ʼ��ָ�򱾵�·��������Ƿ����ϵĵ�ͼ·������ôm_sPath����ָ�����ǹ���Ļ���ͼƬ�� ����·��
	double m_resolution;

	enum VLayerType m_layerType; //ͼ������
	void setLayerType(enum VLayerType layerType){ m_layerType = layerType; }

	bool m_bVisible;
	void setVisiable(bool flag){ m_bVisible = flag;}
	bool isVisiable(){ return m_bVisible;}
};

