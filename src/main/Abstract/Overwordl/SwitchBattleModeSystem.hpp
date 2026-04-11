#pragma once

#pragma once
#include "Abstract/ECS/System/System.hpp"

struct SwitchBattleModeSystem : System {

	SwitchBattleModeSystem(ArchetypeManager &manager);
	void update() override;
};