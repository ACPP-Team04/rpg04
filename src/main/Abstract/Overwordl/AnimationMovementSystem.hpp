#pragma once
#include "Abstract/ECS/System/System.hpp"

struct AnimationMovementSystem : System {

	explicit AnimationMovementSystem(ArchetypeManager &manager);
	void update() override;
};