#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

struct DialogSystem : System {

	sf::RenderWindow &window;
	tgui::Gui &gui;
	DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window,tgui::Gui &gui);

	void update() override;
};