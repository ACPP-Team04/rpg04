#pragma once
#include "../ComponentRegistry/ComponentRegistry.hpp"
#include "Abstract/EntityID.hpp"

#include <bitset>
#include <iostream>
#include <ostream>
#include <strings.h>
#include <typeindex>
#include <unordered_set>
#include <vector>

class ArchetypeBitSignature {

	public:
	static constexpr int MAX_COMPONENTS = 64;
	template <typename ...T>
	void convertToBits()
	{
		(this->setBit(ComponentRegistry::getInstance().getComponentId<T>()),...);
	}

	public:
	auto getTypeSignature() const {
		return this->bits;
	}
	  friend bool operator==(const ArchetypeBitSignature &lhs, const ArchetypeBitSignature &rhs)
	  {
		  return lhs.bits == rhs.bits;
	  }
	  friend bool operator!=(const ArchetypeBitSignature &lhs, const ArchetypeBitSignature &rhs)
	  {
		  return !(lhs == rhs);
	  }

	static std::vector<int> getIntersectionIds(ArchetypeBitSignature a, ArchetypeBitSignature b)
	{
		std::vector<int> result;
		std::bitset<MAX_COMPONENTS> intersect = a.getTypeSignature() & b.getTypeSignature();
		for (int i = 0; i < MAX_COMPONENTS; i++) {
			if (intersect.test(i)) {
				result.push_back(i);
			}
		}
		return result;
	}

	private:
	std::bitset<MAX_COMPONENTS> bits;
	void setBit(const int bit)
	{

		this->bits.set(bit);

	}




};

struct IPool {
	IPool() = default;
	virtual ~IPool() = default;
	IPool(const IPool &other) = default;
	IPool(IPool &&other) = default;
	IPool &operator=(const IPool &other)=default;
	IPool &operator=(IPool &&other) = default;
	virtual size_t addEntity()=0;
	virtual	void removeLastEntity()=0;
	virtual void moveFrom(size_t indexTo,size_t indexFrom)=0;
	virtual void copyTo(size_t oldIndex, IPool* otherPool, size_t newIndex) = 0;
};

template <typename T>
struct ComponentPool: IPool {
	std::vector<T> components;


	size_t addEntity() override
	{
		this->components.emplace_back();
		return this->components.size()-1;
	}


	void removeLastEntity() override
	{
		this->components.pop_back();
	}


	T& getComponent(size_t location)
	{
		return this->components[location];
	}
	T& getLastEntityComponent()
	{
		return this->components.back();
	}

	void moveFrom(size_t indexTo,size_t indexFrom)
	{
		this->components[indexTo] = std::move(this->components[indexFrom]);
	}

	void copyTo(size_t oldIndex, IPool *newPool, size_t newIndex) override
	{
		std::cout << oldIndex << " " << newIndex << std::endl;
		auto* newPoolCast = static_cast<ComponentPool<T>*>(newPool);
		newPoolCast->components[newIndex] = std::move(this->components[oldIndex]);
	}
};


class Archetype {
	public:
	template <typename ...T>
	static Archetype createArchType(ArchetypeBitSignature signature)
	{
		Archetype archetype = Archetype();
		archetype.init<T...>(signature);
		return archetype;
	}
	friend bool operator==(const Archetype &lhs, const Archetype &rhs) { return lhs.type == rhs.type; }
	friend bool operator!=(const Archetype &lhs, const Archetype &rhs) { return !(lhs == rhs); }


	IPool* getPoolById(int componentId)
	{
		return componentPools[componentId].get();
	}
	template <typename T>
	ComponentPool<T>* getComponentPool() {
		int id = ComponentRegistry::getInstance().getComponentId<T>();
		return static_cast<ComponentPool<T>*>(componentPools[id].get());
	}

	ArchetypeBitSignature getTypeSignature() const
	{
		return this->type;
	}

	template <typename T>
	int getComponentId()
	{
		ComponentRegistry registry = ComponentRegistry::getInstance();
		int id = registry.getComponentId<T>();
		return id;
	}

	size_t addEntity(EntityID id) {
		entityIds.push_back(id);
		for (auto const& [id, pool] : componentPools) {
			pool->addEntity();
		}
		return getLastEntityIndex();
	}

	void removeEntityAt(size_t location) {
		size_t lastIndex = entityIds.size() - 1;
		if (location != lastIndex) {
			entityIds[location] = entityIds.back();
		}
		entityIds.pop_back();
		for (auto const& [id, pool] : componentPools) {
			pool->moveFrom(location,lastIndex);
			pool->removeLastEntity();
		}

	}

	EntityID getLastEntityId()
	{
		return this->entityIds.back();
	}

	size_t getLastEntityIndex()
	{
		return entityIds.size()-1;
	}

	private:
	ArchetypeBitSignature type;
	std::unordered_map<int,std::unique_ptr<IPool>> componentPools;
	std::vector<EntityID> entityIds;

	template <typename ...T>
	void init(ArchetypeBitSignature signature)
	{
		this->type = signature;
		this->createComponentVectors<T...>();
	}

	template <typename ...T>
	void createComponentVectors()
	{
		(createComponentVector<T>(),...);

	};

	template <typename T>
	void createComponentVector()
	{
		ComponentRegistry registry = ComponentRegistry::getInstance();
		int componentId = registry.getComponentId<T>();
		this->componentPools.insert({componentId,std::make_unique<ComponentPool<T>>()});

	}
};

namespace std {
	template <>
	struct hash<ArchetypeBitSignature> {
		size_t operator()(const ArchetypeBitSignature &archetypeBitSignature) const
		{
			return hash<std::bitset<ArchetypeBitSignature::MAX_COMPONENTS>>{}(archetypeBitSignature.getTypeSignature());
		}
	};
	template <>
	struct hash<std::shared_ptr<Archetype>> {
		size_t operator()(const std::shared_ptr<Archetype> &archetype) const
		{
			return hash<ArchetypeBitSignature>{}(archetype->getTypeSignature());
		}
	};
};