#pragma once
#include "PartOfLayerComponent.hpp"

struct AnimationPartComponent : public Component<AnimationPartComponent> {
	void readFromJson(tson::TiledClass &j) override {}
};