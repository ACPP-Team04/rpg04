#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "tileson.h"
#include <SFML/Graphics/Font.hpp>


struct BoundIngBoxComponent : public Component<BoundIngBoxComponent> {
	sf::FloatRect bounds;
	void readFromJson(tson::TiledClass &j) override
	{

	}
};