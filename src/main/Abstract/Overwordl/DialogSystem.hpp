#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct DialogSystem : System {

	sf::RenderWindow &window;
	DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window);

	void update() override;
};