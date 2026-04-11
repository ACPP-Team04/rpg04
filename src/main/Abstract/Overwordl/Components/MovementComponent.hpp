#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct MovementComponent : public Component<MovementComponent> {
	float speed;

	void readFromJson(const nlohmann::json &j) override { this->speed = j.value("speed", 1.0f); };
};
