#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "DefaultComponent.hpp"
#include <SFML/Graphics.hpp>
struct TransformComponent : public Component<TransformComponent>,DefaultComponent {
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

	void readFromObject(tson::Object &object,ParseContext &context)
	{
		position.x = (float)object.getPosition().x;
		position.y = (float)object.getPosition().y;
		setRotation(object.getRotation());
		scale.x = (float)object.getSize().x;
		float sx = (float)object.getSize().x / (float)context.tileSize.x;
		float sy = (float)object.getSize().y / (float)context.tileSize.y;
		scale.x = (sx > 0.f) ? sx : 1.f;
		scale.y = (sy > 0.f) ? sy : 1.f;

	}
};
