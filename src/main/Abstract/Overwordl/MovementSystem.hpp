#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct MovementSystem : System {
	MovementSystem(ArchetypeManager &manager);
	void update() override;
};