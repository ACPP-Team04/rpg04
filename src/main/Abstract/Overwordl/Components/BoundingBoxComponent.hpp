#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "DefaultComponent.hpp"
#include "tileson.h"
#include <SFML/Graphics/Font.hpp>

struct BoundIngBoxComponent : public Component<BoundIngBoxComponent>,DefaultComponent {
	sf::FloatRect bounds;
	void readFromJson(tson::TiledClass &j) override {}

	void readFromObject(tson::Object &object, ParseContext &context) override
	{

	}
};