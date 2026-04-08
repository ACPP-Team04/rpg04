#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/Components.hpp"
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
}

int main()
{

	sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");

	ECSManager ecsManager = ECSManager(window);
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