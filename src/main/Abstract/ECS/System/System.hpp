#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"

struct System {

	ArchetypeManager& manager;

	System(ArchetypeManager& manager) : manager(manager) {};
	virtual ~System() = default;

	virtual void update() = 0;

};