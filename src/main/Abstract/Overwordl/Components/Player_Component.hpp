#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct PlayerComponent : public Component<PlayerComponent> {

	void readFromJson(const nlohmann::json &j) override {}
};