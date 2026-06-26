#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"

struct DoorSystem : public System {
	explicit DoorSystem(ArchetypeManager &manager);

	void update() override;
};