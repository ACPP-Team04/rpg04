//
// Created by marcocotrotzo on 26.03.26.
//

#include "Abstract/ECS/ECSRepository.hpp"

#include <unordered_map>
#include <vector>

class SlowRepository:public ECSRepository {

  public:
	SlowRepository(const SlowRepository &other) : components(other.components) {}
	SlowRepository():ECSRepository(){};

  private:
	std::unordered_map<Entity, std::reference_wrapper<Component>> components;
	void addComponentToEntityImpl(const Entity& entity, Component&& component) override
	{

		components.emplace(entity, component);
	};
	void removeComponentFromEntityImpl(const Entity& entity,Component& component) override
	{
		components.erase(entity);
	};
};