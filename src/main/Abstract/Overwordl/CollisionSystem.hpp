#pragma once
#include "Abstract/ECS/System/System.hpp"

struct CollisionSystem : public System {
	CollisionSystem(ArchetypeManager &manager);
	void update() override;
};