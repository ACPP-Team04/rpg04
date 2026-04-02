#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct CameraSystem : System {

	sf::RenderWindow &window;
	CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window);
	void update() override;
};