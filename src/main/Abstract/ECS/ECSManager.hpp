#pragma once
#include "Abstract/Overwordl/BoundingBoxSystem.hpp"
#include "Abstract/Overwordl/CameraSystem.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"

#include "Abstract/Overwordl/DialogSystem.hpp"
#include "Abstract/Overwordl/InputSystem.hpp"
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include "Abstract/Overwordl/ItemSystem.hpp"
#include "Abstract/Overwordl/MovementSystem.hpp"
#include "Abstract/Overwordl/RenderSystem.hpp"
#include "Abstract/Overwordl/SwitchLayerSystem.hpp"
#include "Archetype/ArchetypeManager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>


struct ECSManager {

	sf::RenderWindow &window;
	ArchetypeManager manager = ArchetypeManager();
	RenderSystem renderSystem;
	InputSystem inputSystem;
	MovementSystem movementSystem;
	tgui::Gui gui;
	CameraSystem cameraSystem;
	SwitchLayerSystem switchLayerSystem;
	DialogSystem dialogSystem;
	InteractionSystem interactionSystem;
	CollisionSystem collisionSystem;
	BoundingBoxSystem boundingBoxSystem;
	ItemSystem	item_system;
	ECSManager(sf::RenderWindow &window)
	    : window(window), renderSystem(manager, window), inputSystem(manager, window), movementSystem(manager),
	      cameraSystem(manager, window), switchLayerSystem(manager), collisionSystem(manager),
	      dialogSystem(manager, window), interactionSystem(manager),boundingBoxSystem(manager),
	item_system(manager)
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
		boundingBoxSystem.update();
		inputSystem.update();
		movementSystem.update();
		boundingBoxSystem.update();
		collisionSystem.update();
		boundingBoxSystem.update();
		interactionSystem.update();
		switchLayerSystem.update();
		cameraSystem.update();
		renderSystem.update();
		dialogSystem.update();
		item_system.update();

	}
};
