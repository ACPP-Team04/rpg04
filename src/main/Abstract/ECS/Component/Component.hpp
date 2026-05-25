#pragma once
#include "ComponentTypeInfo.hpp"
#include "tileson.h"
#include <algorithm>

template <typename T>
class Component {
  public:
	Component() = default;
	virtual ~Component() = default;
	TypeInfo<T> typeInfo;
	virtual void readFromJson(tson::TiledClass &j) = 0;
};