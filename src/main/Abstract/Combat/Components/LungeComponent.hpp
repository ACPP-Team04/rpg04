#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include <SFML/System/Vector2.hpp>

struct LungeComponent : public Component<LungeComponent> {
	sf::Vector2f originalPosition;
	sf::Vector2f targetPosition;

	int originalLayerGroup;

	int framesElapsed = 0;
	int totalFrames = 20;
	EntityID targetEntity;
	virtual void readFromJson(tson::TiledClass &j) override {
		// Intentionally empty:
		// This component is strictly internal and is not parsed directly from Tiled
	};
};