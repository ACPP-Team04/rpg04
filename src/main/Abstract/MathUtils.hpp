#pragma once

#include "Overwordl/Components/BoundingBoxComponent.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

static bool collides(const sf::FloatRect &bounds, const sf::FloatRect &bounds2)
{
	return bounds.findIntersection(bounds2).has_value();
}

static bool collides(const BoundIngBoxComponent &bounds, const BoundIngBoxComponent &bounds2)
{
	return collides(bounds.bounds, bounds2.bounds);
}

static float distance(const sf::FloatRect &bounds, const sf::FloatRect &bounds2)
{
	auto centerA = bounds.getCenter();
	auto centerB = bounds2.getCenter();
	float dx = centerA.x - centerB.x;
	float dy = centerA.y - centerB.y;

	return std::sqrt((dx * dx + dy * dy));
}

static sf::Rect<float> collidesOverlap(const sf::FloatRect &rectA, const sf::FloatRect &rect)
{
	return rectA.findIntersection(rect).value();
}

static bool isinRadius(const sf::FloatRect rectA, const sf::FloatRect rectB, const float &radius)
{

	return distance(rectA, rectB) <= radius;
}

static bool isinRadius(const BoundIngBoxComponent &rectA, const BoundIngBoxComponent &rectB, const float &radius)
{
	return isinRadius(rectA.bounds, rectB.bounds, radius);
}