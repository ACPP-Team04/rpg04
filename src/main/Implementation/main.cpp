#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/UI/MainMenu.hpp"

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
}

int main()
{
	sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(logicalSize.x), static_cast<unsigned>(logicalSize.y)}),
	                        "Zombie Knight");
	tgui::Gui gui(window);

	window.setFramerateLimit(60);

	registerComponents();
	ECSManager ecsManager = ECSManager(window, gui);
	WorldParser parser = WorldParser(ecsManager.manager, window);
	parser.update();
	ecsManager.init();
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
