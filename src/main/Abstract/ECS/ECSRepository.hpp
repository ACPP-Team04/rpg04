#pragma once
#include "Component.hpp"
#include "Entity.hpp"

#include <typeindex>
#include <typeinfo>
#include <utility>
class ECSRepository
{
	public:
	ECSRepository()=default;
	virtual ~ECSRepository() = default;
};