#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct AIComponent : public Component<AIComponent> {
	AIComponent() = default;
	void readFromJson(const nlohmann::json &j) override {};
};
