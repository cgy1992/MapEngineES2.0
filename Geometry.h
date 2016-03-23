#pragma once
#include "DataStruct.h"
class CGeometry
{
public:
	CGeometry(void);
	~CGeometry(void);

	int m_id;
	enum VGeometryType m_type;
};

