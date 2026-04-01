#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;

	void readFromJson(const nlohmann::json &j) override
	{
		this->position.x = j.value("x", 0.0f);
		this->position.y = j.value("y", 0.0f);
		this->scale.x = j.value("scale_x", 1.0f);
		this->scale.y = j.value("scale_y", 1.0f);
		this->rotation = sf::degrees(j.value("rotation", 0.0f));

	}
};

struct KeyState {
	bool pressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton;

	void readFromJson(const nlohmann::json &j) override
	{
		this->moveUp = KeyState(j.value("up",false));
		this->moveDown = KeyState(j.value("down",false));
		this->moveRight = KeyState(j.value("right",false));
		this->moveLeft = KeyState(j.value("left",false));
		this->menuButton = KeyState(j.value("button",false));
	}
};

struct RenderComponent : public Component<RenderComponent> {
	TileType activeTiles;
	void readFromJson(const nlohmann::json &j) override
	{
		this->activeTiles = j.value("activeTiles", TILE_HOUSE1_BROWN);
	}
};


struct WorldComponent : public Component<WorldComponent> {
	int tilewidth;
	int tileheight;
	int width;
	int height;
	bool active;
	void readFromJson(const nlohmann::json &j) override
	{
		tilewidth = j.value("tilewidth",16);
		tileheight = j.value("tileheight",16);
		width = j.value("width",30);
		height = j.value("height",30);
	}
};
