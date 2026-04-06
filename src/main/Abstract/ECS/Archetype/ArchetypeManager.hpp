#pragma once
#include "Abstract/ECS/Entity/EntityTag.hpp"
#include "Archetype.hpp"
#include "View.hpp"

#include <SFML/Window/Keyboard.hpp>
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
	std::unordered_map<EntityTag, std::vector<EntityID>> entityTagToEntityId;
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
		return entityIdToArchetype[entityId].archetype->getArchTypeSignature() == archetype->getArchTypeSignature();
	}

	void addEntityTag(EntityTag tag, EntityID entity_id)
	{
		if (!this->entityTagToEntityId.contains(tag)) {
			this->entityTagToEntityId.insert({tag, {}});
		}
		this->entityTagToEntityId[tag].push_back(entity_id);
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
		addEntityIdsToArchType(entityId, newArchType);
	}

	void addEntityIdsToArchType(EntityID entityId, SharedArchetype newArchType)
	{
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
	View<T...> view()
	{
		ArchetypeBitSignature requestSignature = ArchetypeBitSignature::get<T...>();
		View<T...> view;
		std::vector<SharedArchetype> intersectionArchetypes;
		for (auto [signature, arch] : this->archetypes) {
			if (ArchetypeBitSignature::intersect(requestSignature, signature) == requestSignature) {
				intersectionArchetypes.push_back(arch);
			}
		}
		if (intersectionArchetypes.empty()) {
			return view;
		}
		view.archetypes = intersectionArchetypes;
		return view;
	}

	template <typename... T>
	EntityID createEntity(EntityTag tag)
	{
		EntityID entityId = EntityID();
		this->addEntityIdsToArchType<T...>(entityId);
		this->addEntityTag(tag, entityId);
		return entityId;
	}
	template <typename... T>
	EntityID createEntity()
	{
		EntityID entityId = EntityID();
		this->addEntityIdsToArchType<T...>(entityId);
		return entityId;
	}

	std::vector<EntityID> getEntityIdByTag(EntityTag tag) { return this->entityTagToEntityId[tag]; }

	template <typename T>
	T &getComponent(EntityID entityId)
	{
		EntityLocation location = getEntityLocation(entityId);
		return std::get<0>(location.archetype->getComponentArrays<T>(location.index));
	}
	template <typename T>
	bool hasComponent(EntityID entityId)
	{
		try {
			getComponent<T>(entityId);
			return true;
		}
		catch (std::exception* e) {
			return false;
		}
		catch (std::exception& e) {
			return false;
		}
	}

	template <typename... T>
	void removeComponentFromEntity(EntityID entityId)
	{
		if (!hasArchetype(entityId)) {
			throw std::runtime_error("Cannot remove component from entity because it has no archetype!");
		}

		EntityLocation location = getEntityLocation(entityId);
		ArchetypeBitSignature oldArchTypeSignature = location.archetype->getArchTypeSignature();
		ArchetypeBitSignature removeSignature = ArchetypeBitSignature::get<T...>();
		ArchetypeBitSignature newArchTypeSignature =
		    ArchetypeBitSignature(oldArchTypeSignature.signature & (~removeSignature.signature));

		SharedArchetype oldArchetype = this->getArchetypeBySignature(oldArchTypeSignature);
		SharedArchetype newArchetype = Archetype::createEmptyArchTypeBySignature(newArchTypeSignature);

		oldArchetype->getComponentArea()->copyStructureTo(newArchetype->getComponentArea(), newArchTypeSignature);
		addArchtypeBySignature(newArchetype);
		this->addEntityIdsToArchType(entityId, newArchetype);
	}

	template <typename... T>
	void addComponentToEntity(EntityID entityId)
	{
		EntityLocation location = getEntityLocation(entityId);
		SharedArchetype oldArchetype = location.archetype;
		SharedArchetype newArchetype = oldArchetype->addComponent<T...>();

		ArchetypeBitSignature newSig = newArchetype->getArchTypeSignature();
		if (signatureHasArchetype(newSig)) {
			newArchetype = getArchetypeBySignature(newSig);
		} else {
			addArchtypeBySignature(newArchetype);
		}
		this->addEntityIdsToArchType(entityId, newArchetype);
	}
	EntityTag getEntityTag(EntityID entityId)
	{
		for (auto &[tag, entityIds] : this->entityTagToEntityId) {
			if (std::find(entityIds.begin(), entityIds.end(), entityId) != entityIds.end()) {
				return tag;
			}
		}
		throw std::runtime_error("No tag found for entity");
	}
};
