#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct PlayerComponent : public Component<PlayerComponent> {

	void readFromJson(tson::TiledClass &j) override {}
};