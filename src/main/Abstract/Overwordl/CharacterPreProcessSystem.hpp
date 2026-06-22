#pragma once
#include "Abstract/ECS/System/System.hpp"

struct CharacterPreProcessSystem : System {

	explicit CharacterPreProcessSystem(ArchetypeManager &manager);

	void update() override;
};