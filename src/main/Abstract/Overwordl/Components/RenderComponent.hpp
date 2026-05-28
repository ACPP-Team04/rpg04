#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "DefaultComponent.hpp"

struct RenderComponent : public Component<RenderComponent> ,DefaultComponent {

	int z_layer;
	void readFromJson(tson::TiledClass &j) override {}
	void readFromObject(tson::Object &object, ParseContext &context) override{};
};
