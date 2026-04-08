#pragma once
#include "../Component/Component.hpp"

class EntityID {
  public:
	EntityID() : id(IdCounter++) {}
	~EntityID() = default;
	EntityID(const EntityID &other) : id(other.id) {}
	EntityID(EntityID &&other) noexcept : id(other.id) {}
	EntityID &operator=(const EntityID &other)
	{
		if (this == &other)
			return *this;
		id = other.id;
		return *this;
	}
	EntityID &operator=(EntityID &&other) noexcept
	{
		if (this == &other)
			return *this;
		id = other.id;
		return *this;
	}

	inline static int IdCounter = 0;

	int getId() const { return this->id; }
	EntityID(int explicitId) : id(explicitId)
	{
		if (explicitId >= IdCounter)
			IdCounter = explicitId + 1;
	}

  private:
	int id;

	friend bool operator==(const EntityID &a, const EntityID &b) { return a.id == b.id; }

	friend bool operator!=(const EntityID a, const EntityID &b) { return a.id != b.id; }
};

namespace std {
template <>
struct hash<EntityID> {
	size_t operator()(const EntityID &entityId) const { return hash<int>{}(entityId.getId()); }
};
} // namespace std