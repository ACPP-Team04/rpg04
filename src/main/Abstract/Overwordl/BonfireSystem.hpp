#pragma once
#include "Abstract/ECS/System/System.hpp"

struct BonfireSystem : System {

	explicit BonfireSystem(ArchetypeManager &manager);
	void update() override;
};