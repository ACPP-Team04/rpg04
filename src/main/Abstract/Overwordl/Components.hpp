#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale;
	sf::Angle rotation = sf::Angle::Zero;
};

struct KeyState {
	bool pressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton;
};

struct RenderComponent : public Component<RenderComponent> {
	TileType activeTile;
	void setActiveTile(const TileType tile) { activeTile = tile; }
};
