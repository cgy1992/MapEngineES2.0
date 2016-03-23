#include "Element.h"


CElement::CElement(DrawEngine *den)
{
	//m_Geometry = NULL;
	m_unit = L"";
	m_visible = true;
	this->den = den;
}


CElement::~CElement(void)
{
}
