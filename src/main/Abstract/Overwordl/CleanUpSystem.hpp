#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"

struct CleanUpSystem : public System {
	CleanUpSystem(ArchetypeManager &manager);
	void update() override;
};