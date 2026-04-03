#pragma once
#include "ComponentTypeInfo.hpp"
#include <nlohmann/json.hpp>

template <typename T>
class Component {
  public:
	Component() = default;
	virtual ~Component() = default;
	TypeInfo<T> typeInfo;
	virtual void readFromJson(const nlohmann::json &j) = 0;

};