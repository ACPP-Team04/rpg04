#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
struct HudSystem : public System {
	sf::RenderWindow &window;
	tgui::Gui &gui;
	HudSystem(ArchetypeManager &manager, sf::RenderWindow &window, tgui::Gui &gui);

	int elpasedFramesSinceLastMessage = 0;
	void update() override;
};