#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/Components.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"

#include <SFML/Graphics.hpp>


int main()
{

	sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");

	ECSManager ecsManager = ECSManager(window);
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