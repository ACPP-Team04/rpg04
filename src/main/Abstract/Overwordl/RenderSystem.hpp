#pragma once
#include "Abstract/ECS/System/System.hpp"
#include "Components/WorldComponent.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct RenderSystem : System {

	sf::RenderWindow &window;
	RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window);
	void renderGrid(WorldComponent *world);
	void renderTiles(WorldComponent * world);
	void update() override;
};