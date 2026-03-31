#include "Abstract/ECS/ECSManager.hpp"

#include <SFML/Graphics.hpp>


int main()
{
	sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");
	ECSManager ecsManager = ECSManager(window);
	while (window.isOpen())
	{
		ecsManager.update();
		window.display();

	}
}