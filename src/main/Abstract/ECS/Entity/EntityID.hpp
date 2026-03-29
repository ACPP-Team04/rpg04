#pragma once
#include "../Component/Component.hpp"

class EntityID{
	public:
	inline static int IdCounter = 0;

	int getId() const
	{
		return this->id;
	}
	private:
	int id = IdCounter++;

	friend bool operator==(const EntityID& a,const EntityID& b)
	{
		return a.id == b.id;
	}

	friend bool operator!=(const EntityID a, const EntityID& b)
	{
		return a.id != b.id;
	}
};

namespace std {
	template <>
	struct hash<EntityID> {
		size_t operator()(const EntityID &entityId) const
		{
			return hash<int>{}(entityId.getId());
		}
	};
}