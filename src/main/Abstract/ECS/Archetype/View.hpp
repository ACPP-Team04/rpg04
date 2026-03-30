#pragma once
#include "Archetype.hpp"
#include <vector>
template <typename... T>
struct View {

	std::vector<SharedArchetype> archetypes;
	template <typename Function>
	void each(Function &&function)
	{
		for (SharedArchetype archetype : this->archetypes) {
			EntityID *entityIds = archetype->getEntityList();
			auto allComponentArrays = archetype->getComponentArrays<T...>();

			for (int i = 0; i < archetype->getNumOfEntities(); i++) {
				function(entityIds[i], (std::get<T *>(allComponentArrays)[i])...);
			}
		}
	}
};