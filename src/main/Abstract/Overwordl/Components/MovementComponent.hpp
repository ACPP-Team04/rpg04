#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct MovementComponent : public Component<MovementComponent> {
	float speed;

	void readFromJson(tson::TiledClass &j) override

	{
		this->speed = j.get<float>("speed");
	};
};
