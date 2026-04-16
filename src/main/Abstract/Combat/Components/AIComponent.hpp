#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct AIComponent : public Component<AIComponent> {
	AIComponent() = default;
	virtual void readFromJson(tson::TiledClass &j) override {};
};
