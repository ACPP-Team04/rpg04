#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct DialogSystem;

struct MovementSystem : System {
	DialogSystem &dialogSystem;

	MovementSystem(ArchetypeManager &manager, DialogSystem &dialogSystem);
	void update() override;
};
