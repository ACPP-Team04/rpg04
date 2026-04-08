#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/Window/Keyboard.hpp>

struct InteractionComponent : public Component<InteractionComponent> {

	bool isActive = false;
	INTERACTION_TRIGGER trigger;
	INTERACTION_ACTION action;
	sf::Keyboard::Key interactionKey;
	bool inRange = false;
	float focusRadius;

	void readFromJson(const nlohmann::json &j) override
	{
		isActive = false;
		trigger = j.value("trigger", INTERACTION_TRIGGER());
		action = j.value("action", INTERACTION_ACTION());
		focusRadius = j.value("focusRadius", 1.0f);
		interactionKey = j.value("interactionKey", sf::Keyboard::Key());
	}
};
