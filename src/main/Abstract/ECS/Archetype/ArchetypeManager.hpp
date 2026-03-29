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
	std::unordered_map<ArchetypeBitSignature, SharedArchetype> archetypes;
	std::unordered_map<EntityID, EntityLocation> entityIdToArchetype;
	SharedArchetype getArchetypeBySignature(ArchetypeBitSignature signature) { return this->archetypes[signature]; }

	void addArchtypeBySignature(SharedArchetype archetype)
	{
		this->archetypes.insert({archetype->getArchTypeSignature(), archetype});
	}

	bool signatureHasArchetype(ArchetypeBitSignature signature) { return this->archetypes.contains(signature); }

	bool hasArchetype(EntityID entityId) { return this->entityIdToArchetype.contains(entityId); }

	EntityLocation getEntityLocation(EntityID entityId) { return this->entityIdToArchetype[entityId]; }

	void setEntityLocation(EntityID entityId, EntityLocation newLocation)
	{
		this->entityIdToArchetype[entityId] = newLocation;
	}

	void deleteEntityLocation(EntityID entityId) { this->entityIdToArchetype.erase(entityId); }

	bool isEntityInArchetype(EntityID entityId, SharedArchetype archetype)
	{
		if (!entityIdToArchetype.contains(entityId)) {
			return false;
		}
		return entityIdToArchetype[entityId].archetype == archetype;
	}

	template <typename... T>
	SharedArchetype createArchType()
	{
		SharedArchetype newArchType = Archetype::createArchType<T...>();
		addArchtypeBySignature(newArchType);
		return newArchType;
	}

	template <typename... T>
	SharedArchetype createArchTypeIfNotExists()
	{
		ArchetypeBitSignature archeTypeSignature = ArchetypeBitSignature::get<T...>();
		if (signatureHasArchetype(archeTypeSignature)) {
			return getArchetypeBySignature(archeTypeSignature);
		}
		return createArchType<T...>();
	}

	void removeEntityIdFromArchetype(EntityID deletableEntityID, SharedArchetype archetype)
	{
		EntityLocation oldLocation = getEntityLocation(deletableEntityID);
		EntityID entityIdOnOldLocation = archetype->removeEntity(oldLocation.index);
		setEntityLocation(entityIdOnOldLocation, oldLocation);
		deleteEntityLocation(deletableEntityID);
	}

	template <typename... T>
	void addEntityIdsToArchType(EntityID entityId)
	{
		SharedArchetype newArchType = createArchTypeIfNotExists<T...>();

		if (!hasArchetype(entityId)) {
			size_t location = newArchType->registerEntity(entityId);
			setEntityLocation(entityId, {newArchType, location});
			return;
		}

		if (isEntityInArchetype(entityId, newArchType)) {
			return;
		}

		migrateToNewArchetype(entityId, newArchType);
	}

	void migrateToNewArchetype(EntityID entityId, SharedArchetype newArchetype)
	{
		EntityLocation oldLocation = getEntityLocation(entityId);
		size_t newIndex = newArchetype->moveComponentsFromArchetype(entityId, oldLocation.index, oldLocation.archetype);
		removeEntityIdFromArchetype(entityId, oldLocation.archetype);
		this->setEntityLocation(entityId, {newArchetype, newIndex});
	}

  public:
	template <typename... T>
	EntityID createEntity()
	{
		EntityID entityId = EntityID();
		this->addEntityIdsToArchType<T...>(entityId);
		return entityId;
	}

	template <typename... T>
	void addComponent(EntityID entityId)
	{
		this->addEntityIdsToArchType<T...>(entityId);
	}
};