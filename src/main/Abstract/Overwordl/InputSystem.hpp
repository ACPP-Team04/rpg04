#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct InputSystem : public System {
	InputSystem(ArchetypeManager &manager, sf::RenderWindow &window);

	void update() override;
};