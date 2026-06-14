#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/CharacterPreProcessSystem.hpp"
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
#include "Abstract/RegisterService.hpp"
#include "Abstract/UI/GameOverMenu.hpp"
#include "Abstract/UI/MainMenu.hpp"
#include <Abstract/Combat/Components/CombatGodMode.hpp>
#include <Abstract/GameConfig/GameConfig.hpp>
#include <Abstract/Overwordl/Components/PersistanceComponent.hpp>
#include <Abstract/Persistance/SaveManager.hpp>
#include <SFML/Graphics.hpp>
#include <exception>

namespace {
constexpr sf::Vector2f logicalSize{WORLD_SIZE_X, WORLD_SIZE_Y};
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
} // namespace

void readGameConfig()
{
	std::string configPath = std::string(ROOT_DIR) + "/src/ressources/config.json";
	GameConfig::getInstance().loadConfig(configPath);
}

void applyGameConfig(ECSManager &ecsManager, EntityID player)
{
	if (GameConfig::getInstance().isGodModeEnabled()) {
		ecsManager.manager.addComponentToEntity<CombatGodMode>(player);
		spdlog::info("God Mode applied to player!");
	}
	auto mappings = GameConfig::getInstance().getMusicMappings();
	WorldComponent *world = WorldUtils::getWorld(ecsManager.manager);
	if (world) {
		world->groupMusicMap = mappings;
	}
}

void initializeEngine(ArchetypeManager &manager)
{
	manager.subscribeToDestruction([&manager](EntityID id) {
		if (manager.hasComponent<PersistanceComponent>(id)) {
			std::string uuid = manager.getComponent<PersistanceComponent>(id).uuid;
			PersistenceManager::getInstance().deadUniqueEntities.insert(uuid);
			spdlog::info("Observer: Recorded {} as dead.", uuid);
		}
	});
}

void executeLoadSequence(ArchetypeManager &manager, WorldParser &parser,
                         PersistenceRegistrationSystem &registrationSystem, CharacterPreProcessSystem &preprocess,
                         int slotIndex)
{
	spdlog::info("Executing load sequence for Slot {}...", slotIndex);

	nlohmann::json saveData;
	try {
		saveData = SaveManager::loadSaveFile(slotIndex);
	} catch (const std::exception &e) {
		spdlog::error("Aborting load: Save file error: {}", e.what());
		return;
	}

	manager.clear();
	parser.update();
	registrationSystem.update();
	preprocess.update();

	SaveManager::applyWorldStateOverrides(manager);
	auto playerOpt = WorldUtils::getPlayer(manager);
	if (!playerOpt.has_value()) {
		spdlog::critical("WorldParser failed to spawn a default player!");
		throw std::runtime_error("WorldParser failed to spawn a default player!");
	}
	SaveManager::injectWorldComponent(manager, saveData["worldState"]);

	SaveManager::injectPlayer(manager, saveData["player"], playerOpt.value());
	if (saveData["worldState"].contains("doorStates")) {
		SaveManager::injectDoors(manager, saveData["worldState"]["doorStates"]);
	}

	if (saveData["worldState"].contains("dialogStates")) {
		SaveManager::injectDialogs(manager, saveData["worldState"]["dialogStates"],
		                           saveData["worldState"]["interactionStates"]);
	}
	std::string savedMusic = saveData["worldState"].value("currentMusic", "");
	if (!savedMusic.empty()) {
		AudioManager::getInstance().playMusic(savedMusic, true);
	}
	spdlog::info("Load sequence completely finished!");
}

int main()
{
#ifndef _DEBUG
	try {
#endif
		sf::RenderWindow window(
		    sf::VideoMode({static_cast<unsigned>(logicalSize.x), static_cast<unsigned>(logicalSize.y)}),
		    "Zombie Knight");
		tgui::Gui gui(window);
		gui.setFont(FONT);
		readGameConfig();

		spdlog::info("Creating ECS manager...");
		ECSManager ecsManager = ECSManager(window, gui);
		spdlog::info("Initializing Systems...");
		ecsManager.init();
		initializeEngine(ecsManager.manager);
		auto player = WorldUtils::getPlayer(ecsManager.manager);
		if (!player.has_value()) {
			throw std::runtime_error("Startup failed: no player entity was created by WorldParser.");
		}
		applyGameConfig(ecsManager, player.value());
		WorldUtils::playMusicForCurrentGroup(ecsManager.manager);
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
			auto &persistence = PersistenceManager::getInstance();

			if (persistence.requestSave) {
				SaveManager::saveGame(ecsManager.manager, 1);
				persistence.requestSave = false;
			}

			if (persistence.requestLoad) {
				executeLoadSequence(ecsManager.manager, ecsManager.worldParser,
				                    ecsManager.persistanceRegistrationSystem, ecsManager.character_preprocess_system,
				                    1);
				persistence.requestLoad = false;
			}

			if (persistence.requestQuit) {
				gameState = GameState::Quit;
				persistence.requestQuit = false;
			}
			if (persistence.requestGameOver) {
				gameState = GameState::GameOver;
				GameOverMenu::setUpGameOverMenu(gui, gameState);
				persistence.requestGameOver = false;
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
				world->addPersistentMessage("FPS: " + std::to_string(fps));
			}
		}
#ifndef _DEBUG
	} catch (const std::exception &e) {
		spdlog::critical("Fatal startup/runtime error: {}", e.what());
		return 1;
	} catch (...) {
		spdlog::critical("Fatal startup/runtime error: unknown exception");
		return 1;
	}
#endif
	return 0;
}
