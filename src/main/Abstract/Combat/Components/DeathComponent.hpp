#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct DeathComponent : public Component<DeathComponent> {
	DeathComponent() = default;
	virtual void readFromJson(tson::TiledClass &j) override {};
};
