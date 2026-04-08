#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct KeyState {
	bool pressed = false;
	bool justPressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton, interact;

	void readFromJson(const nlohmann::json &j) override
	{
		this->moveUp = KeyState(j.value("up", false));
		this->moveDown = KeyState(j.value("down", false));
		this->moveRight = KeyState(j.value("right", false));
		this->moveLeft = KeyState(j.value("left", false));
		this->menuButton = KeyState(j.value("button", false));
		this->interact = KeyState(j.value("interact", false));
	}
};