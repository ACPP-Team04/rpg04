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
		SharedArchetype newArchType = std::make_shared<Archetype>(Archetype::createArchType<T...>(archeTypeSignature));
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
		EntityID lastEntityId = archetype->getLastEntityId();
		archetype->removeEntityAt(deletableEntityLocation.index);
		this->entityIdToArchetype[lastEntityId].index = deletableEntityLocation.index;
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

		size_t newLocationIdx = newArchType->addEntity(entityId);
		moveInterSectionComponents(oldLocation.index,newLocationIdx,oldLocation.archetype, newArchType);

		removeEntityIdFromArchetype(entityId,oldLocation.archetype);
		entityIdToArchetype[entityId] = {newArchType, newLocationIdx};
	}


	void moveInterSectionComponents(int oldIndex, int newIndex, SharedArchetype oldArchetype,SharedArchetype newArchetype)
	{
		auto intersectionIds = ArchetypeBitSignature::getIntersectionIds(oldArchetype->getTypeSignature(), newArchetype->getTypeSignature());

		for (auto intersectionId : intersectionIds) {
			IPool *newPool = newArchetype->getPoolById(intersectionId);
			IPool *oldPool = oldArchetype->getPoolById(intersectionId);
			oldPool->copyTo(oldIndex,newPool,newIndex);
		}
	}

  public:

	template <typename ...T>
	EntityID createEntity()
	{
		EntityID entityId = EntityID();
		this->addEntityIdsToArchType<T...>(entityId);
		return entityId;
	}

	template <typename ...T>
	void addComponent(EntityID entityId)
	{
		this->addEntityIdsToArchType<T...>(entityId);
	}



};