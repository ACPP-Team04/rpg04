#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/UI/MainMenu.hpp"

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/AnimationPartComponent.hpp"
#include "Abstract/Overwordl/Components/BoundingBoxComponent.hpp"
#include "Abstract/Overwordl/Components/CameraComponent.hpp"
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

#include <Abstract/Audio/AudioManager.hpp>
#include <Abstract/Combat/Components/CombatGodMode.hpp>
#include <Abstract/GameConfig/GameConfig.hpp>
#include <Abstract/Overwordl/Components/AudioComponent.hpp>
#include <Abstract/Overwordl/Components/START_EQUIPMENT_COMPONENT.hpp>
#include <SFML/Graphics.hpp>

namespace {
constexpr sf::Vector2f logicalSize{800.f, 600.f};
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

void registerComponents()
{
	ComponentRegistry::getInstance().registerComponent<MovementComponent>("MOVEMENT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CameraComponent>("CAMERA_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<WorldComponent>("WORLD_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InputComponent>("INPUT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<SwitchLayerComponent>("SWITCH_LAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CollisionComponent>("COLLISION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<NPC_Component>("NPC_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<DialogComponent>("DIALOG_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InteractionComponent>("INTERACTION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<PlayerComponent>("PLAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BoundIngBoxComponent>("BOUNDING_BOX_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ItemComponent>("ITEM_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InventoryComponent>("INVENTORY_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<IsLockedComponent>("LOCKED_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ITEM_HEALSTATS_COMPONENT>("ITEM_HEALSTATS_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BattleComponent>("BATTLE_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BattleManagerComponent>("BattleManagerComponent");
	ComponentRegistry::getInstance().registerComponent<WeaponComponent>("ITEM_WEAPON_STATS_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<StatsComponent>("STATS_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<START_EQUIPMENT_COMPONENT>("EQUIPMENT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<AudioComponent>("AUDIO_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<AnimationPartComponent>("ANIMATION_SPRITE_COMPONENT");
}

void registerAudio()
{
	AssetManager::getInstance().registerMusic("overworld", std::string(ROOT_DIR)
	                                                           + "/src/ressources/audio/music/the_field_of_dreams.ogg");
	AssetManager::getInstance().registerMusic("combat",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/music/battleThemeA.ogg");
	AssetManager::getInstance().registerSound("victory_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/combat_victory.wav");
	AssetManager::getInstance().registerSound("defeat_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/death.wav");
	AssetManager::getInstance().registerSound("light_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/punch_1b.wav");
	AssetManager::getInstance().registerSound("heavy_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/punch_alt-2a.wav");
	AssetManager::getInstance().registerSound("ultimate_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/foom_0.wav");
	AssetManager::getInstance().registerSound("heal_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/healspell3.wav");
	AssetManager::getInstance().registerSound("rest_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/short_wind.wav");
	AssetManager::getInstance().registerSound("enemy_death_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/zombie_death.wav");
	AssetManager::getInstance().registerSound("test",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/zombie_death.wav");
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
	sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(logicalSize.x), static_cast<unsigned>(logicalSize.y)}),
	                        "Zombie Knight");
	tgui::Gui gui(window);

	window.setFramerateLimit(60);

	registerComponents();
	AudioManager audioManager;
	ECSManager ecsManager = ECSManager(window, audioManager, gui);
	WorldParser parser = WorldParser(ecsManager.manager, window);
	parser.update();
	ecsManager.init();
	registerAudio();
	audioManager.playMusic("overworld", true);
	auto player = WorldUtils::getPlayer(ecsManager.manager);
	// ecsManager.manager.addComponentToEntity<CombatGodMode>(player.value());
	applyGameConfig(ecsManager, player.value());
	window.setFramerateLimit(60);

	applyResize(window, gui, ecsManager);

	auto gameState = GameState::MainMenu;
	setUpMainMenu(gui, gameState);
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
	}
}
