#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct RenderComponent : public Component<RenderComponent> {

	int z_layer;
	void readFromJson(tson::TiledClass &j) override {}
};
