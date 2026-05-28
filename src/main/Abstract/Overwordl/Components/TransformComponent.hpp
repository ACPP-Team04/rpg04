#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "DefaultComponent.hpp"
#include <SFML/Graphics.hpp>
struct TransformComponent : public Component<TransformComponent>,DefaultComponent {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;
	sf::Vector2f previousPosition;
	sf::Vector2f size;

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
		size.x = (float)object.getSize().x;
		size.y = (float)object.getSize().y;
		if (object.getGid() > 0)
		{
			position.y -= size.y;
		}
		setRotation(object.getRotation());
		float sx = size.x / context.tileSize.x;
		float sy = size.y / context.tileSize.y;
		scale.x = (sx > 0.f) ? sx : 1.f;
		scale.y = (sy > 0.f) ? sy : 1.f;

	}

	sf::FloatRect getBoundingBox() const
	{
		return {position, size};
	}
	sf::FloatRect getBoundingBox(float offsetX, float offsetY) const
	{
		return {{position.x - offsetX, position.y - offsetY},
				{size.x + offsetX * 2, size.y + offsetY * 2}};
	}

};
