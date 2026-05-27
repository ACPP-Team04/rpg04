#pragma once
#include "../Component/Component.hpp"

class EntityID {
  public:
	~EntityID() = default;
	EntityID ()
	{
		id = -1;
	}
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

	static EntityID fromExistingId(int entityId)
	{
		EntityID e(entityId);
		return e;
	}
	static EntityID createWithId(int explicitId)
	{
		if (explicitId >= IdCounter)
			IdCounter = explicitId + 1;

		EntityID e(explicitId);
		return e;

	}

	static EntityID create()
	{
		EntityID e(IdCounter++);
		return e;
	}

	inline static int IdCounter = 0;

	int getId() const { return this->id; }


  private:
	EntityID(int idNew) : id(idNew) {}
	int id;
	friend bool operator==(const EntityID &a, const EntityID &b) { return a.id == b.id; }

	friend bool operator!=(const EntityID a, const EntityID &b) { return a.id != b.id; }
	friend bool operator==(const EntityID &a, int b) { return a.id == b; }
	friend bool operator==(int a, const EntityID &b) { return a == b.id; }
	friend bool operator!=(const EntityID &a, int b) { return a.id != b; }
	friend bool operator!=(int a, const EntityID &b) { return a != b.id; }

};

namespace std {
template <>
struct hash<EntityID> {
	size_t operator()(const EntityID &entityId) const { return hash<int>{}(entityId.getId()); }
};
} // namespace std