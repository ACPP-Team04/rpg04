#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <SFML/Window/Keyboard.hpp>

struct InteractionComponent : public Component<InteractionComponent> {

	bool isActive = false;
	INTERACTION_TRIGGER trigger;
	INTERACTION_ACTION action;
	sf::Keyboard::Key interactionKey;
	bool inRange = false;
	float focusRadius;
	bool mustLeaveRadius = false;

	void readFromJson(tson::TiledClass &j) override
	{
		isActive = false;
		trigger = WorldUtils::getEnumValue<INTERACTION_TRIGGER>(j, "trigger");
		action = WorldUtils::getEnumValue<INTERACTION_ACTION>(j, "action");
		focusRadius = j.get<float>("focusRadius");
		interactionKey = WorldUtils::getEnumValue<sf::Keyboard::Key>(j, "interactionKey");

		int n = 10;
	}
};
