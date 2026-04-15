#pragma once
#include <algorithm>
#include "ComponentTypeInfo.hpp"
#include "tileson.h"


template <typename T>
class Component {
  public:
	Component() = default;
	virtual ~Component() = default;
	TypeInfo<T> typeInfo;
	virtual void readFromJson(tson::TiledClass &j) = 0;
};