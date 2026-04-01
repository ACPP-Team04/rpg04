#pragma once
#include "Archetype/ArchetypeManager.hpp"
#include "System/System.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

struct ECSManager {

	sf::RenderWindow &window;
	ArchetypeManager manager = ArchetypeManager();
	tgui::Gui gui;

	ECSManager(sf::RenderWindow &window) : window(window), gui(window) {}

	~ECSManager() = default;
	void processEvents()
	{
		while (const std::optional event = window.pollEvent()) {
			gui.handleEvent(*event);
			if (event->is<sf::Event::Closed>())
				window.close();
		}
	}
	void update() { processEvents(); }
};
