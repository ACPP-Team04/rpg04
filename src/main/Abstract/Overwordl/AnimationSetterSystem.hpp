#pragma once
#include "Abstract/ECS/System/System.hpp"

struct AnimationSetterSystem : System {
	explicit AnimationSetterSystem(ArchetypeManager &manager);

	void update() override;
};