#pragma once
#include "ECSRepository.hpp"

#include <iostream>
#include <memory>
#include <ostream>

class SlowRepository : public ECSRepository {
	private:
	std::unordered_map<Entity,std::shared_ptr<Component>> components;

	~SlowRepository() override = default;

	public:
	template <typename T>
	std::shared_ptr<Component> addComponentToEntity(Entity entity)
	{
		std::shared_ptr<Component> component = std::make_shared<T>();
		this->components[entity] = component;

		return component;
	}
	template <typename T>
	std::optional<std::shared_ptr<T>> getComponentByEntity(Entity entity)
	{
		auto it = this->components.find(entity);
		if (it == this->components.end()) {
			return std::nullopt;
		}
		auto casted = std::dynamic_pointer_cast<T>(it->second);
		if (!casted) {
			return std::nullopt; 
		}
		return casted;
	}

};