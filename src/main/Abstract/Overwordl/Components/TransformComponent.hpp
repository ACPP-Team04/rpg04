#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;
	sf::Vector2f previousPosition;

	void readFromJson(const nlohmann::json &j) override
	{

		this->position.x = j.value("position_x", 0.0f);
		this->position.y = j.value("position_y", 0.0f);
		this->scale.x = j.value("scale_x", 1.0f);
		this->scale.y = j.value("scale_y", 1.0f);
		this->rotation = sf::degrees(j.value("rotation", 0.0f));
	}
};
