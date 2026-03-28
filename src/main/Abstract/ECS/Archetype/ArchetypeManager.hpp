#pragma once
#include "Archetype.hpp"

#include <unordered_map>
#include <unordered_set>



using SharedArchetype = std::shared_ptr<Archetype>;
struct EntityLocation {
	SharedArchetype archetype;
	size_t index;
};
class ArchetypeManager {
	std::unordered_map<ArchetypeBitSignature,SharedArchetype> archetypes;
	std::unordered_map<EntityID,EntityLocation> entityIdToArchetype;
	template <typename ...T>

	SharedArchetype addArchType()
	{
		ArchetypeBitSignature archeTypeSignature = ArchetypeBitSignature();
		archeTypeSignature.convertToBits<T...>();
		if (this->archetypes.contains(archeTypeSignature)) {
			return this->archetypes[archeTypeSignature];
		}
		SharedArchetype newArchType = std::make_shared<Archetype>(Archetype::createArchType<T...>);
		this->archetypes.insert({archeTypeSignature, newArchType});

		return newArchType;
	}

	bool hasArchetype(EntityID entityId)
	{
		return this->entityIdToArchetype.contains(entityId);
	}
	void removeEntityIdFromArchetype(EntityID deletableEntityID,SharedArchetype archetype)
	{
		EntityLocation deletableEntityLocation = this->entityIdToArchetype[deletableEntityID];
		archetype->removeEntityAt(deletableEntityLocation.index);

		EntityID lastEntityId = archetype->getLastEntityId();
		this->entityIdToArchetype[archetype->getLastEntityId()].index=deletableEntityLocation.index;
		this->entityIdToArchetype.erase(deletableEntityID);
	}
	template <typename ...T>
	void addEntityIdsToArchType(EntityID entityId)
	{
		SharedArchetype newArchType = addArchType<T...>();
		if (!hasArchetype(entityId)) {
			size_t location = newArchType->addEntity(entityId);
			entityIdToArchetype[entityId] = {newArchType, location};
			return;
		}

		EntityLocation oldLocation = this->entityIdToArchetype[entityId];
		if (oldLocation.archetype == newArchType) {
			return;
		}
		removeEntityIdFromArchetype(entityId,oldLocation.archetype);

		size_t newIdx = targetArch->addEntity(entityId);
		entityIdToArchetype[entityId] = {targetArch, newIdx};
	}





	template <typename T>
	size_t addEntityIdToArchetype(SharedArchetype archetype)
	{
		ComponentPool<T>*componentPool = archetype->getComponentPool<T>();
		return componentPool->addEntity();

	}

	EntityID getEntityLocationByIndex(ArchetypeBitSignature bitSignature,size_t index)
	{
		for (auto it : this->entityIdToArchetype) {
			if (it.second.archetype.get()->getTypeSignature() != bitSignature) {
				continue;
			}
			if (it.second.index == index) {
				return it.first;
			}
		}
		throw std::out_of_range("Entity index out of range");
	}

  public:
	template <typename ...T>
	EntityID createArchType()
	{
		EntityID entityId = EntityID();
		std::shared_ptr<Archetype> newArchType = addArchType<T...>();
		newArchType.get()->addEntityIdToComponentMap(entityId);

	}

	std::shared_ptr<Archetype> addComponent(EntityID entityId)
	{

	}
};