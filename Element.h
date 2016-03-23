#pragma once
#include <string>
#include "stdafx.h"
#include "Geometry.h"
#include "DrawEngine.h"

using namespace std;
class CElement
{
public:
	CElement(DrawEngine *den = NULL);
	~CElement(void);

	int m_id;
	string m_name;
	bool m_visible;
	CString m_unit;

	CGeometry m_Geometry;

	virtual void RenderUseOgles(){};
	DrawEngine *den;
};

