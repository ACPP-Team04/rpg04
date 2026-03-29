#pragma once
#include "ComponentTypeInfo.hpp"

template <typename T>
class Component {
	public:
	Component() = default;
	virtual ~Component() = default;
	TypeInfo<T> typeInfo;
};