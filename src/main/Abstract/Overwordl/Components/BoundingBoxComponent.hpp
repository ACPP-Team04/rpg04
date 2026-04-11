#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

struct BoundIngBoxComponent : public Component<BoundIngBoxComponent> {
	sf::FloatRect bounds;
	void readFromJson(const nlohmann::json &j) override {}
};