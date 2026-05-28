#pragma once
#include "Abstract/ECS/System/System.hpp"

struct CharacterPreProcessSystem:System {

	CharacterPreProcessSystem(ArchetypeManager &manager);

	void update() override;
};