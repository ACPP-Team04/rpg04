#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct RenderComponent : public Component<RenderComponent> {

	int z_layer;
	void readFromJson(const nlohmann::json &j) override {}
};
