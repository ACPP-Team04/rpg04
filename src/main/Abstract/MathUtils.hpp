#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>
static bool collides(const sf::FloatRect &bounds, const sf::FloatRect &bounds2)
{
	return bounds.findIntersection(bounds2).has_value();
}

static std::optional<sf::Rect<float>> collidesWithValue(const sf::FloatRect &bounds, const sf::FloatRect &bounds2)
{
	return bounds.findIntersection(bounds2);
}

static float distance(const sf::FloatRect &bounds, const sf::FloatRect &bounds2)
{
	auto centerA = bounds.getCenter();
	auto centerB = bounds2.getCenter();
	float dx = centerA.x - centerB.x;
	float dy = centerA.y - centerB.y;
	return std::sqrt((dx * dx + dy * dy));
}

static bool isinRadius(const sf::FloatRect rectA, const sf::FloatRect rectB, const float &radius)
{
	return distance(rectA, rectB) <= radius;
}