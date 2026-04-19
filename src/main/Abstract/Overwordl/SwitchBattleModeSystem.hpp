#pragma once

#pragma once
#include "Abstract/ECS/System/System.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

struct SwitchBattleModeSystem : System {

	SwitchBattleModeSystem(ArchetypeManager &manager);
	void update() override;
	[[nodiscard]] std::vector<EntityID> getEnemiesInRatio(const sf::Vector2f center, float radius, EntityID playerId);
	void prepareForBattle(const std::vector<EntityID> &participants, EntityID playerId);
	static float getSquaredDistance(const sf::Vector2f &a, const sf::Vector2f &b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return (dx * dx) + (dy * dy);
	}
};
