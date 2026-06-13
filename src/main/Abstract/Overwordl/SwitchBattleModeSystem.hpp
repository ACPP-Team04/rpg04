#pragma once

#pragma once
#include "Abstract/ECS/System/System.hpp"
#include <Abstract/Audio/AudioSystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

struct SwitchBattleModeSystem : System {

	SwitchBattleModeSystem(ArchetypeManager &manager, AudioSystem &audioSystem);
	void update() override;
	[[nodiscard]] std::vector<EntityID> getEnemiesInRatio(const EntityID &initialEnemy, const sf::Vector2f center,
	                                                      float radius, const std::vector<EntityID> &playerParty);
	void preparePlayerPartyForBattle(const std::vector<EntityID> &participants, EntityID playerId);
	void prepareEnemiesForBattle(const std::vector<EntityID> &enemies);
	static float getSquaredDistance(const sf::Vector2f &a, const sf::Vector2f &b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return (dx * dx) + (dy * dy);
	}
	AudioSystem &audioSystem;
};
