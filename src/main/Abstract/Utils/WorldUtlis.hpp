#pragma once
#include <algorithm>
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "tileson.h"

#include <magic_enum/magic_enum.hpp>
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
		return (world->currentLevel == level_name);
	}

	static WorldComponent *getWorld(ArchetypeManager &manager)
	{
		WorldComponent *world = nullptr;

		manager.view<WorldComponent>().each([&](auto id, auto &comp) {
			world = &comp;
		});
		if(world == nullptr) {
			throw std::runtime_error("No world component found!");
		}

		return world;
	}

	static bool isPartOfCurrentLayer(ArchetypeManager &manager, const EntityID &entity)
	{
		if (!manager.hasComponent<PartOfLayerComponent>(entity)) {
			throw std::runtime_error("Entity does not have the PartOfLayer component!");
		}
		const auto &pComp = manager.getComponent<PartOfLayerComponent>(entity);
		return isCurrentLayer(manager, pComp.layer, pComp.level);
	}
	template <typename ...T, typename Function>
	static void viewInCurrentLayer(ArchetypeManager &manager,Function &&function)
	{

		manager.view<T...>().each([&](auto &entity, T&... components) {
			if (!isPartOfCurrentLayer(manager, entity)) {
				return;
			}
			function(entity, components...);
		});
	}

	static std::optional<EntityID> getPlayer(ArchetypeManager &manager)
	{
		auto players = getPlayers(manager);
		if (players.empty())
			throw std::runtime_error("No player found. You need a player with player component in the current layer!");
		return players[0];
	}

	static std::vector<EntityID> getPlayers(ArchetypeManager &manager)
	{
		std::vector<EntityID> result;
		viewInCurrentLayer<PlayerComponent>(manager,[&](auto &entity, auto &component) { result.push_back(entity); });
		return result;
	}


	template <typename T>
	static std::optional<std::reference_wrapper<T>> getPlayersComponent(ArchetypeManager &manager)
	{
		auto player = getPlayer(manager);
		if (!player.has_value()) {
			throw std::runtime_error("No player found");
		}
		if (!manager.hasComponent<T>(player.value())) {
			throw std::runtime_error("Player does not have the requested component");
		}
		return std::ref(manager.getComponent<T>(player.value()));
	}

	template <typename T>
	static T getEnumValue(tson::TiledClass propClass, std::string name)
	{
		auto prop = propClass.getMember(name);
		std::string enumString = prop->getValue<std::string>();

		return magic_enum::enum_cast<T>(enumString).value_or(T{});
	}
};
