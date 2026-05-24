#pragma once
#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Systems/StatsDistributorSystem.hpp"
#include "Abstract/Overwordl/AnimationMovementSystem.hpp"
#include "Abstract/Overwordl/AnimationSetterSystem.hpp"
#include "Abstract/Overwordl/BoundingBoxSystem.hpp"
#include "Abstract/Overwordl/CameraSystem.hpp"
#include "Abstract/Overwordl/CleanUpSystem.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"

#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Overwordl/DialogSystem.hpp"
#include "Abstract/Overwordl/DoorSystem.hpp"
#include "Abstract/Overwordl/HudSystem.hpp"
#include "Abstract/Overwordl/InputSystem.hpp"
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include "Abstract/Overwordl/ItemSystem.hpp"
#include "Abstract/Overwordl/MenuSystem.hpp"
#include "Abstract/Overwordl/MovementSystem.hpp"
#include "Abstract/Overwordl/RenderSystem.hpp"
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"
#include "Abstract/Overwordl/SwitchLayerSystem.hpp"
#include "Archetype/ArchetypeManager.hpp"

#include <Abstract/Audio/AudioSystem.hpp>
#include <Abstract/Combat/Systems/EnemyHealthBarSystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

// Call escManager.init() after construction to initialize the battle UI layout and callbacks
struct ECSManager {

	sf::RenderWindow &window;
	tgui::Gui gui;
	ArchetypeManager manager = ArchetypeManager();
	AudioManager &audioManager;
	AudioSystem audioSystem;
	RenderSystem renderSystem;
	InputSystem inputSystem;
	MovementSystem movementSystem;
	CameraSystem cameraSystem;
	BattleInputSystem battleInputSystem;
	AISystem aiSystem;
	CombatSystem combatSystem;
	StatsDistributorSystem statsDistributorSystem;

	SwitchLayerSystem switchLayerSystem;
	DialogSystem dialogSystem;
	InteractionSystem interactionSystem;
	CollisionSystem collisionSystem;
	BoundingBoxSystem boundingBoxSystem;
	MenuSystem menuSystem;
	ItemSystem item_system;
	DoorSystem door_system;
	SwitchBattleModeSystem switch_battle_mode_system;
	EnemyHealthBarSystem enemyHealthBarSystem;
	AnimationSetterSystem animation_setter_system;
	AnimationMovementSystem animation_movement_system;
	CleanUpSystem clean_up_system;
	HudSystem hudSystem;

	ECSManager(sf::RenderWindow &window, AudioManager &audioManager)
	    : window(window), gui(window), manager(), audioManager(audioManager), audioSystem(manager, audioManager),
	      renderSystem(manager, window), inputSystem(manager, window), movementSystem(manager),
	      cameraSystem(manager, window), switchLayerSystem(manager), collisionSystem(manager),
	      dialogSystem(manager, window, gui), interactionSystem(manager), boundingBoxSystem(manager),
	      item_system(manager), menuSystem(manager, gui), door_system(manager), battleInputSystem(manager, gui, window),
	      aiSystem(manager), combatSystem(manager, aiSystem, audioSystem), statsDistributorSystem(manager, gui),
	      switch_battle_mode_system(manager, audioSystem), enemyHealthBarSystem(manager, gui, window),
	      animation_movement_system(manager), animation_setter_system(manager), clean_up_system(manager),
	      hudSystem(manager, window, gui)

	{
		gui.setWindow(window);
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
	bool checkMenu()
	{
		bool menuOpened = false;
		this->manager.view<WorldComponent>().each(
		    [&](auto entityId, WorldComponent &worldComponent) { menuOpened = worldComponent.menuOpened; });
		return menuOpened;
	}

	void init() { battleInputSystem.init(); }

	void update()
	{
		processEvents();
		window.clear(sf::Color::Transparent);
		hudSystem.update();
		boundingBoxSystem.update();
		inputSystem.update();
		movementSystem.update();
		animation_movement_system.update();
		boundingBoxSystem.update();
		collisionSystem.update();
		boundingBoxSystem.update();
		interactionSystem.update();
		door_system.update();
		switchLayerSystem.update();
		boundingBoxSystem.update();
		cameraSystem.update();
		animation_setter_system.update();
		renderSystem.update();
		switch_battle_mode_system.update();
		battleInputSystem.update();
		combatSystem.update();
		enemyHealthBarSystem.update();
		boundingBoxSystem.update();
		statsDistributorSystem.update();
		dialogSystem.update();
		item_system.update();
		audioSystem.update();
		clean_up_system.update();
		gui.draw();
	}
};
