#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

#include <SFML/Graphics.hpp>


int main()
{
	sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");
	ECSManager ecsManager = ECSManager(window);
	EntityID entity_id = ecsManager.manager.createEntity<RenderComponent,TransformComponent,InputComponent>();
	ecsManager.manager.getComponent<RenderComponent>(entity_id).setActiveTile(TileType::PLAYER_IDLE_FRONT);
	ecsManager.manager.getComponent<TransformComponent>(entity_id).scale = sf::Vector2f{15,15};
	while (window.isOpen())
	{
		window.clear();

		ecsManager.update();
		window.display();
	}
}