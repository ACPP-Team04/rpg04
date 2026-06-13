#pragma once
#include "Abstract/Audio/AudioManager.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "tileson.h"
#include <algorithm>

#include <magic_enum/magic_enum.hpp>
class WorldUtils {
  public:
	static WorldComponent *getWorld(ArchetypeManager &manager)
	{
		WorldComponent *world = nullptr;

		manager.view<WorldComponent>().each([&](auto id, auto &comp) { world = &comp; });
		if (world == nullptr) {
			throw std::runtime_error("No world component found!");
		}

		return world;
	}

	template <typename... T, typename Function>
	static void viewInCurrentLayer(ArchetypeManager &manager, Function &&function)
	{
		WorldComponent *world = getWorld(manager);

		manager.view<T..., PartOfLayerComponent>().each(
		    [&](auto &entity, T &...components, PartOfLayerComponent &layer) {
			    if (world->currentGroup != layer.groupId) {
				    return;
			    }
			    function(entity, components...);
		    });
	}

	template <typename... T, typename Function>
	static void viewInSpecificLayer(ArchetypeManager &manager, int groupId, Function &&function)
	{
		manager.view<T..., PartOfLayerComponent>().each(
		    [&](auto &entity, T &...components, PartOfLayerComponent &layer) {
			    if (groupId != layer.groupId) {
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
		manager.view<PlayerComponent>().each(([&](auto &entity, auto &component) { result.push_back(entity); }));
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
			return std::nullopt;
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

	static void playMusicForCurrentGroup(ArchetypeManager &manager)
	{
		WorldComponent *world = getWorld(manager);
		if (!world)
			return;

		auto &audio = AudioManager::getInstance();
		int currentGroup = world->currentGroup;

		if (world->groupMusicMap.contains(currentGroup)) {
			std::string targetTrack = world->groupMusicMap[currentGroup];
			auto currentTrackOpt = audio.getCurrentMusicName();
			if (!currentTrackOpt.has_value() || currentTrackOpt.value() != targetTrack) {
				spdlog::info("Switching music to: {}", targetTrack);
				audio.playMusic(targetTrack, true);
			}
		} else {
			spdlog::info("No music mapping found for group {}", currentGroup);
			audio.stopMusic();
		}
	}
};
