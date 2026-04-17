#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct KeyState {
	bool pressed = false;
	bool justPressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton, interact;

	void readFromJson(tson::TiledClass &j) override
	{
		this->moveUp = KeyState(j.get<bool>("up"));
		this->moveDown = KeyState(j.get<bool>("down"));
		this->moveRight = KeyState(j.get<bool>("right"));
		this->moveLeft = KeyState(j.get<bool>("left"));
		this->menuButton = KeyState(j.get<bool>("button"));
		this->interact = KeyState(j.get<bool>("interact"));
	}
};