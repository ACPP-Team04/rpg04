#pragma once
#include "Abstract/ECS/System/System.hpp"

struct AnimationMovementSystem : System {

	AnimationMovementSystem(ArchetypeManager &manager);
	void update() override;
};