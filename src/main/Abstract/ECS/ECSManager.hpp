#pragma once
#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Systems/StatsDistributorSystem.hpp"
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
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

struct ECSManager {

	sf::RenderWindow &window;
	ArchetypeManager manager = ArchetypeManager();
	RenderSystem renderSystem;
	InputSystem inputSystem;
	MovementSystem movementSystem;
	tgui::Gui gui;
	CameraSystem cameraSystem;
	BattleInputSystem battleInputSystem;
	AISystem aiSystem;
	CombatSystem combatSystem;
	StatsDistributorSystem statsDistributorSystem;

	SwitchLayerSystem switchLayerSystem;
	CollisionSystem collisionSystem;
	DialogSystem dialogSystem;
	InteractionSystem interactionSystem;
	ECSManager(sf::RenderWindow &window)
	    : window(window), renderSystem(manager, window), inputSystem(manager, window), movementSystem(manager),
	      cameraSystem(manager, window), gui(window), battleInputSystem(manager, gui), aiSystem(manager),
	      combatSystem(manager, aiSystem), statsDistributorSystem(manager, gui)
	{
	}

	~ECSManager() = default;
	void processEvents()
	{
		while (const std::optional event = window.pollEvent()) {
			gui.handleEvent(*event);
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
		battleInputSystem.update();
		combatSystem.update();
		statsDistributorSystem.update();
		gui.draw();
		dialogSystem.update();
	}
};
