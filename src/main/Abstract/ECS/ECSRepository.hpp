#pragma once
#include "Component.hpp"
#include "Entity.hpp"

#include <typeindex>
#include <typeinfo>
#include <utility>
class ECSRepository
{
	public:
	ECSRepository();
	virtual ~ECSRepository() = 0;

	template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>>* = nullptr>
	T& addComponentToEntity(const Entity& entity)
	{
		return addComponentToEntityImpl(entity,T());
	};
	template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>>* = nullptr>
	T& removeComponentFromEntity(const Entity& entity)
	{
		return removeComponentFromEntityImpl(entity,T());
	}


  private:
		virtual void addComponentToEntityImpl(const Entity& entity,Component& component) = 0;
		virtual void removeComponentFromEntityImpl(const Entity& entity, Component& component)=0;
};