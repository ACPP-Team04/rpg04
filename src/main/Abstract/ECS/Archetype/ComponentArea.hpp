#pragma once
#include "../Entity/EntityID.hpp"
#include "ArchetypeBitSignature.hpp"
#include "ComponentPool.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
struct ComponentArea {
	std::unordered_map<ArchetypeBitSignature, std::unique_ptr<IPool>> componentPools;
	std::vector<EntityID> entityIds;
	size_t registerEntity(EntityID entityId)
	{
		for (auto& it:this->componentPools) {
			it.second->addEntity();
		}
		entityIds.emplace_back(entityId);
		return getLastEntityIndex();
	}

	size_t getLastEntityIndex() const
	{
		return this->entityIds.size()-1;
	}

	EntityID removeEntity(size_t index)
	{
		size_t lastIndex = getLastEntityIndex();
		EntityID lastEntityId = this->entityIds[lastIndex];

		if (index != lastIndex) {
			entityIds[index] = entityIds.back();
		}
		entityIds.pop_back();

		for (auto const& [id, pool] : componentPools) {
			pool->moveFrom(index,lastIndex);
			pool->removeLastEntity();
		}

		return lastEntityId;
	}

	template <typename ...T>
	void createComponentVectors()
	{
		(createComponentVector<T>(),...);

	}
	template <typename T>
	void createComponentVector()
	{
		this->componentPools.insert({ArchetypeBitSignature::get<T>(),std::make_unique<ComponentPool<T>>()});

	}

	size_t moveEntityFromOldArchetype(EntityID entity_id,size_t oldIndex, ComponentArea * oldComponentArea)
	{
		size_t newIndex = this->registerEntity(entity_id);
		for (auto& [sig, pool] : this->componentPools) {
			if (oldComponentArea->componentPools.contains(sig)) {
				oldComponentArea->componentPools[sig]->copyTo(oldIndex, pool.get(), newIndex);
			}
		}
		return newIndex;
	}
};