#pragma once
#include "Abstract/ECS/System/System.hpp"

struct BoundingBoxSystem:System {

	BoundingBoxSystem(ArchetypeManager &manager);
	void update() override;
};