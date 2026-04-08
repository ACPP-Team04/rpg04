#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"

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
#include "Abstract/Overwordl/Components/ItemWeaponStatsComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/SwitchLayerComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"

#include <SFML/Graphics.hpp>


void registerComponents()
{
	ComponentRegistry::getInstance().registerComponent<TransformComponent>("TRANSFORM_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<MovementComponent>("MOVEMENT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CameraComponent>("CAMERA_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<WorldComponent>("WORLD_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InputComponent>("INPUT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<RenderComponent>("RENDER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<SwitchLayerComponent>("SWITCH_LAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<SpriteComponent>("SPRITE_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CollisionComponent>("COLLISION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<NPC_Component>("NPC_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<DialogComponent>("DIALOG_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InteractionComponent>("INTERACTION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<PlayerComponent>("PLAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BoundIngBoxComponent>("BOUNDING_BOX_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ItemComponent>("ITEM_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ITEM_WEAPON_STATS_KOMPONENT>("ITEM_WEAPON_STATS_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InventoryComponent>("INVENTORY_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<IsLockedComponent>("LOCKED_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ITEM_HEALSTATS_COMPONENT>("ITEM_HEALSTATS_COMPONENT");

}

int main()
{

	sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");

	ECSManager ecsManager = ECSManager(window);
	ecsManager.init();
	registerComponents();
	WorldParser parser = WorldParser(ecsManager.manager,window);
	window.clear(sf::Color::Transparent);
	parser.update();

	window.setFramerateLimit(60);
	while (window.isOpen())
	{
		ecsManager.update();

		window.display();
	}
}