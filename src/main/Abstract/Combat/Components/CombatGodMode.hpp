#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct CombatGodMode : public Component<CombatGodMode> {
	CombatGodMode() = default;
	virtual void readFromJson(tson::TiledClass &j) override {};
};