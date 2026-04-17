#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;
	sf::Vector2f previousPosition;

	void readFromJson(tson::TiledClass &j)
	{

	}

	void setRotation(float rotationDegrees)
	{
		this->rotation = sf::degrees(rotationDegrees);
	}
};
