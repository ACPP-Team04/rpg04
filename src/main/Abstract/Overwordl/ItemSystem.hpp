#pragma once
#include "Abstract/ECS/System/System.hpp"

struct ItemSystem : System {

	ItemSystem(ArchetypeManager &manager);
	void update() override;
};