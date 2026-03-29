#pragma once
#include "ComponentArea.hpp"
class Archetype {
  public:
	template <typename... T>
	static std::shared_ptr<Archetype> createArchType()
	{
		std::shared_ptr<Archetype> archetype = std::make_shared<Archetype>();
		archetype->init<T...>(ArchetypeBitSignature::get<T...>());
		return archetype;
	}

	size_t registerEntity(EntityID entityId) { return this->componentArea.registerEntity(entityId); }

	EntityID removeEntity(size_t index) { return this->componentArea.removeEntity(index); }

	size_t moveComponentsFromArchetype(EntityID entity_id, size_t oldIndex, std::shared_ptr<Archetype> oldArchetype)
	{
		return this->componentArea.moveEntityFromOldArchetype(entity_id, oldIndex, oldArchetype->getComponentArea());
	}

	ComponentArea *getComponentArea() { return &this->componentArea; }

	ArchetypeBitSignature getArchTypeSignature() { return this->type; }

  private:
	ComponentArea componentArea;
	ArchetypeBitSignature type;
	template <typename... T>
	void init(ArchetypeBitSignature signature)
	{
		this->componentArea.createComponentVectors<T...>();
		this->type = signature;
	}
};