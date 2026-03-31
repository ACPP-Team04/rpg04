#pragma once
#include "Abstract/ECS/Component/Component.hpp"
struct PositionComponent : public Component<PositionComponent> {

	int x;
	int y;
};


struct SpriteComponent : public Component<SpriteComponent> {
	
};