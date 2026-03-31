#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct RenderSystem : System {

	sf::RenderWindow &window;
	RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window);
	void update() override;
};