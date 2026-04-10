#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

class WorldUtils {
  public:
	static bool isCurrentLayer(ArchetypeManager &manager, LAYERTYPE targetType)
	{
		WorldComponent *world = nullptr;
		manager.view<WorldComponent>().each([&](auto id, auto &comp) { world = &comp; });
		return (world && world->currentLayer == targetType);
	}

	static bool isCurrentLayer(ArchetypeManager &manager, LAYERTYPE targetType, LEVEL_NAME level_name)
	{
		WorldComponent *world = nullptr;
		manager.view<WorldComponent>().each([&](auto id, auto &comp) { world = &comp; });
		return (world && world->currentLayer == targetType && world->currentLevel == level_name);
	}

	static std::optional<EntityID> getPlayer(ArchetypeManager &manager)
	{
		auto players = getPlayers(manager);
		if (players.empty())
			return std::nullopt;
		return players[0];
	}

	static std::vector<EntityID> getPlayers(ArchetypeManager &manager)
	{
		std::vector<EntityID> result;
		manager.view<PlayerComponent>().each([&](auto &entity, auto &component) { result.push_back(entity); });
		return result;
	}

	static bool isPartOfCurrentLayer(ArchetypeManager &manager, const EntityID &entity)
	{
		const auto &pComp = manager.getComponent<PartOfLayerComponent>(entity);
		return isCurrentLayer(manager, pComp.layer, pComp.level);
	}
};
