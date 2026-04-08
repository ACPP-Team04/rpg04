#pragma once
#include "Abstract/ECS/System/System.hpp"

struct InteractionSystem : public System {
	InteractionSystem(ArchetypeManager &manager);

	void update() override;
};