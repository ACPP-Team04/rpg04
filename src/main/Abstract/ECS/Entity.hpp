#pragma once
class Entity {
	public:
	Entity(int id) : id(id) {};
	virtual ~Entity() = default;
	int getUUID() const
	{
		return this->id;
	};
	bool operator==(const Entity& other) const {
		return id == other.id;
	}

	private:
	int id;
};

#include <functional>

template <>
struct std::hash<Entity> {
	size_t operator()(const Entity &e) const noexcept { return std::hash<int>()(e.getUUID()); }
};
