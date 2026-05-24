#pragma once
#include "Abstract/ECS/System/System.hpp"

struct AnimationSetterSystem : System {
	AnimationSetterSystem(ArchetypeManager &manager);

	void update() override;
};