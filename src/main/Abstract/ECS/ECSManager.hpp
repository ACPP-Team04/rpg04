#pragma once
#include "Abstract/Overwordl/CameraSystem.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"
#include "Abstract/Overwordl/DialogSystem.hpp"
#include "Abstract/Overwordl/InputSystem.hpp"
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include "Abstract/Overwordl/MovementSystem.hpp"
#include "Abstract/Overwordl/RenderSystem.hpp"
#include "Abstract/Overwordl/SwitchLayerSystem.hpp"
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
	SwitchLayerSystem switchLayerSystem;
	CollisionSystem collisionSystem;
	DialogSystem dialogSystem;
	InteractionSystem interactionSystem;
	ECSManager(sf::RenderWindow &window)
	    : window(window), renderSystem(manager, window), inputSystem(manager, window), movementSystem(manager),
	      cameraSystem(manager, window), switchLayerSystem(manager),collisionSystem(manager),dialogSystem(manager,window),interactionSystem(manager)
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
		interactionSystem.update();
		movementSystem.update();
		collisionSystem.update();
		switchLayerSystem.update();
		cameraSystem.update();
		renderSystem.update();
		dialogSystem.update();
	}
};
