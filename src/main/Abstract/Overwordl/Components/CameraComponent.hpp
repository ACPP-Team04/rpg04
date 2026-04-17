#pragma once
#include "Abstract/ECS/Component/Component.hpp"

#include <SFML/System/Vector2.hpp>

struct CameraComponent : public Component<CameraComponent> {
	sf::Vector2f center;
	sf::Vector2f scaleSize;

	void readFromJson(tson::TiledClass &j) override
	{
		scaleSize.x = j.get<float>("scale_x");
		scaleSize.y = j.get<float>("scale_y");
	}
};