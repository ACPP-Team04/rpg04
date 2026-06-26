#pragma once
#include "Abstract/ECS/System/System.hpp"

struct InteractionSystem : public System {
	explicit InteractionSystem(ArchetypeManager &manager);

	void update() override;
};