#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct CollisionComponent : public Component<CollisionComponent> {
	COLLISION_ACTION action;
	void readFromJson(const nlohmann::json &j) override { action = (COLLISION_ACTION)j.value("action", 0); }
};