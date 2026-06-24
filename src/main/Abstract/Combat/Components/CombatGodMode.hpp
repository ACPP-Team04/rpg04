#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct CombatGodMode : public Component<CombatGodMode> {
	CombatGodMode() = default;
	void readFromJson(tson::TiledClass &j) override {
		// Intentionally empty:
		// This component is strictly internal and is not parsed directly from Tiled
	};
};