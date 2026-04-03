#pragma once
#include "Abstract/Overwordl/CameraSystem.hpp"
#include "Abstract/Overwordl/InputSystem.hpp"
#include "Abstract/Overwordl/MovementSystem.hpp"
#include "Abstract/Overwordl/RenderSystem.hpp"
#include "Archetype/ArchetypeManager.hpp"
#include "System/System.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

struct ECSManager {

	sf::RenderWindow &window;
	ArchetypeManager manager = ArchetypeManager();
	RenderSystem renderSystem;
	InputSystem inputSystem;
	MovementSystem movementSystem;
	CameraSystem cameraSystem;

	ECSManager(sf::RenderWindow &window)
	    : window(window), renderSystem(manager, window), inputSystem(manager, window), movementSystem(manager),
	      cameraSystem(manager, window)
	{
	}

	~ECSManager() = default;
	void processEvents()
	{
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}
	}

	void update()
	{
		processEvents();
		window.clear(sf::Color::Transparent);
		inputSystem.update();
		movementSystem.update();
		cameraSystem.update();
		renderSystem.update();
	}
};
