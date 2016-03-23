#pragma once
#include "Stdafx.h"
#include "DrawEngine.h"
#include "Layer.h"

typedef enum VLayerType LayerType;
CLayer *SearchByLayerType(const DrawEngine *den,LayerType type);
