#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/RegisterService.hpp"
#include "Abstract/UI/MainMenu.hpp"

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/AnimationPartComponent.hpp"
#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsLockedComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/SwitchLayerComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"
#include <Abstract/Combat/Components/CombatGodMode.hpp>
#include <Abstract/GameConfig/GameConfig.hpp>
#include <SFML/Graphics.hpp>
#include <exception>

namespace {
constexpr sf::Vector2f logicalSize{WORLD_SIZE_X,WORLD_SIZE_Y};
constexpr float TargetRatio = logicalSize.x / logicalSize.y;

sf::FloatRect getLetterboxViewport(sf::Vector2u windowSize)
{
	float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
	float viewportWidth = 1.f;
	float viewportHeight = 1.f;
	float viewportLeft = 0.f;
	float viewportTop = 0.f;

	if (windowRatio > TargetRatio) {
		viewportWidth = TargetRatio / windowRatio;
		viewportLeft = (1.f - viewportWidth) / 2.f;
	} else if (windowRatio < TargetRatio) {
		viewportHeight = windowRatio / TargetRatio;
		viewportTop = (1.f - viewportHeight) / 2.f;
	}

	return sf::FloatRect({viewportLeft, viewportTop}, {viewportWidth, viewportHeight});
}

void applyResize(sf::RenderWindow &window, tgui::Gui &gui, ECSManager &ecsManager)
{
	const sf::FloatRect viewport = getLetterboxViewport(window.getSize());
	ecsManager.setTargetAspect(TargetRatio);
	ecsManager.setViewport(viewport);

	gui.setRelativeViewport(
	    tgui::FloatRect(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y));
	gui.setAbsoluteView(tgui::FloatRect(0.f, 0.f, logicalSize.x, logicalSize.y));
}
}

void applyGameConfig(ECSManager &ecsManager, EntityID player)
{
	std::string configPath = std::string(ROOT_DIR) + "/src/ressources/config.json";
	GameConfig::getInstance().loadConfig(configPath);
	if (GameConfig::getInstance().isGodModeEnabled()) {
		ecsManager.manager.addComponentToEntity<CombatGodMode>(player);
		spdlog::info("God Mode applied to player!");
	}
}

int main()
{
	try {
		sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(logicalSize.x), static_cast<unsigned>(logicalSize.y)}),
		                        "Zombie Knight");
		tgui::Gui gui(window);
		window.setFramerateLimit(60);


		spdlog::info("Creating ECS manager...");
		ECSManager ecsManager = ECSManager(window,gui);
		spdlog::info("Initializing Systems...");
		ecsManager.init();

		auto player = WorldUtils::getPlayer(ecsManager.manager);
		if (!player.has_value()) {
			throw std::runtime_error("Startup failed: no player entity was created by WorldParser.");
		}
		applyGameConfig(ecsManager, player.value());
		window.setFramerateLimit(FRAME_LIMIT);

		applyResize(window, gui, ecsManager);

		auto gameState = GameState::MainMenu;
		setUpMainMenu(gui, gameState);
		sf::Clock fpsClock;
		while (window.isOpen()) {
			window.clear(sf::Color::Black);

			while (const std::optional event = window.pollEvent()) {
				gui.handleEvent(*event);
				if (event->is<sf::Event::Closed>()) {
					window.close();
				}
				if (event->is<sf::Event::Resized>()) {
					applyResize(window, gui, ecsManager);
				}
			}

			if (gameState == GameState::Game) {
				ecsManager.update();
			}

			if (gameState == GameState::Quit) {
				window.close();
			}

			gui.draw();
			window.display();
			if (PEROMANCE_TEST_MODE) {
				float fps = 1.f / fpsClock.restart().asSeconds();
				WorldComponent *world = WorldUtils::getWorld(ecsManager.manager);
				world->addPersistentMessage("FPS: "+std::to_string(fps));
			}
		}
	} catch (const std::exception &e) {
		spdlog::critical("Fatal startup/runtime error: {}", e.what());
		return 1;
	} catch (...) {
		spdlog::critical("Fatal startup/runtime error: unknown exception");
		return 1;
	}

	return 0;
}
