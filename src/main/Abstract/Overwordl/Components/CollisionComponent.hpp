#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

struct CollisionComponent : public Component<CollisionComponent> {
	COLLISION_ACTION action;
	void readFromJson(tson::TiledClass &j) override
	{
		action = WorldUtils::getEnumValue<COLLISION_ACTION>(j, "action");
	}
};