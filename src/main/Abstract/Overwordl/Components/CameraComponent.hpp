#pragma once
#include "Abstract/ECS/Component/Component.hpp"

#include <SFML/System/Vector2.hpp>

struct CameraComponent : public Component<CameraComponent> {
	sf::Vector2f center;
	sf::Vector2f scaleSize;

	void readFromJson(const nlohmann::json &j) override
	{
		scaleSize.x = j.value("scale_x", 1.0f);
		scaleSize.y = j.value("scale_y", 1.0f);
	}
};