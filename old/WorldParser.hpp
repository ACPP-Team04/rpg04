#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/GlobalProperties.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <nlohmann/json.hpp>


struct WorldParser : System {
	sf::RenderWindow &window;
	WorldParser(ArchetypeManager &manager, sf::RenderWindow &window);
	void update() override;


};