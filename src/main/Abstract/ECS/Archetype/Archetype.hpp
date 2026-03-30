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

	static std::shared_ptr<Archetype> createEmptyArchTypeBySignature(ArchetypeBitSignature signature)
	{
		std::shared_ptr<Archetype> archetype = std::make_shared<Archetype>();
		archetype->init(signature);
		return archetype;
	}
	template <typename... T>
	std::shared_ptr<Archetype> addComponent()
	{
		ArchetypeBitSignature newSignature =
		    ArchetypeBitSignature(this->type.signature | ArchetypeBitSignature::get<T...>().signature);
		auto newArchType = createEmptyArchTypeBySignature(newSignature);
		this->componentArea.copyStructureTo(newArchType->getComponentArea(), newSignature);
		newArchType->getComponentArea()->createComponentVectors<T...>();
		return newArchType;
	}

	size_t registerEntity(EntityID entityId) { return this->componentArea.registerEntity(entityId); }

	EntityID removeEntity(size_t index) { return this->componentArea.removeEntity(index); }

	size_t moveComponentsFromArchetype(EntityID entity_id, size_t oldIndex, std::shared_ptr<Archetype> oldArchetype)
	{
		return this->componentArea.moveEntityFromOldArchetype(entity_id, oldIndex, oldArchetype->getComponentArea());
	}

	size_t getNumOfEntities() const { return (this->componentArea.getLastEntityIndex() + 1); }

	EntityID *getEntityList() { return this->componentArea.getEntityList(); }

	template <typename... T>
	std::tuple<T *...> getComponentArrays()
	{
		return this->componentArea.getComponentArraysReferenceAsTuple<T...>();
	}

	template <typename... T>
	std::tuple<T &...> getComponentArrays(size_t location)
	{
		auto componentArrays = getComponentArrays<T...>();
		if (location > this->componentArea.getLastEntityIndex()) {
			throw std::out_of_range("The archetype has not this entity!");
		}
		return std::tie((std::get<T *>(componentArrays)[location])...);
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

	void init(ArchetypeBitSignature signature) { this->type = signature; }
};
using SharedArchetype = std::shared_ptr<Archetype>;