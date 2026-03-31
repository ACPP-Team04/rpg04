#pragma once
#include "Archetype/ArchetypeManager.hpp"
#include "System/System.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

struct ECSManager {

	sf::RenderWindow& window;
	ArchetypeManager manager = ArchetypeManager();

	ECSManager(sf::RenderWindow& window) : window(window) {}

	~ECSManager() = default;
	void processEvents()
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}


	}

	void update()
	{
		processEvents();
	}


};
