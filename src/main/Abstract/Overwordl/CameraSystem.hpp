#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

struct CameraSystem : System {

	sf::RenderWindow &window;
	sf::FloatRect viewport = sf::FloatRect({0.f, 0.f}, {1.f, 1.f});
	float targetAspect = 4.f / 3.f;

	CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window);
	void setTargetAspect(float targetAspect);
	void setViewport(const sf::FloatRect &viewport);
	void update() override;
};
