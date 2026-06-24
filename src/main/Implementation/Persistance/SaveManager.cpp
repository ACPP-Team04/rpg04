#include "Abstract/Persistance/SaveManager.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/PersistanceComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Overwordl/Components/TransformComponent.hpp>

#include "Abstract/Audio/AudioSystem.hpp"
#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <Abstract/Overwordl/Components/DialogComponent.hpp>
#include <Abstract/Overwordl/Components/IsLockedComponent.hpp>
#include <Abstract/Overwordl/Components/ItemComponent.hpp>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <sstream>

std::string SaveManager::getSaveFilePath(int slotIndex)
{
	fs::path rootPath(ROOT_DIR);
	fs::path saveDir = std::string(ROOT_DIR) + "/src/ressources/saves";

	std::error_code ec;
	fs::create_directories(saveDir, ec);
	if (ec) {
		spdlog::error("OS blocked folder creation! Path: {} | Reason: {}", saveDir.string(), ec.message());
	}

	fs::path filePath = saveDir / fmt::format("save_{}.json", slotIndex);
	return filePath.string();
}

bool SaveManager::doesSaveExist(int slotIndex)
{
	return std::filesystem::exists(getSaveFilePath(slotIndex));
}

void SaveManager::saveGame(ArchetypeManager &manager, int slotIndex)
{
	nlohmann::json saveData;
	saveData["slot"] = slotIndex;

	std::stringstream ss;
	ss << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::current_zone()->to_local(std::chrono::system_clock::now()));

	saveData["metadata"]["timestamp"] = ss.str();

	auto playerEntityOpt = WorldUtils::getPlayer(manager);
	if (playerEntityOpt.has_value()) {
		EntityID player = playerEntityOpt.value();

		// Save Position
		auto &trans = manager.getComponent<TransformComponent>(player);
		saveData["player"]["position"]["x"] = trans.position.x;
		saveData["player"]["position"]["y"] = trans.position.y;

		// Save Stats
		CharacterComponent &characterComp = manager.getComponent<CharacterComponent>(player);
		saveData["player"]["stats"]["experience"] = characterComp.stats.experience;
		saveData["player"]["stats"]["experienceLevel"] = characterComp.stats.experienceLevel;
		saveData["player"]["stats"]["numberOfFightsWon"] = characterComp.stats.numberOfFightsWon;
		saveData["player"]["stats"]["health"] = characterComp.stats.health;
		saveData["player"]["stats"]["STRENGTH"] = characterComp.stats.getStat(STRENGTH);
		saveData["player"]["stats"]["DEXTERITY"] = characterComp.stats.getStat(DEXTERITY);
		saveData["player"]["stats"]["FAITH"] = characterComp.stats.getStat(FAITH);
		saveData["player"]["stats"]["MAX_HEALTH"] = characterComp.stats.getStat(MAX_HEALTH);

		// Save your Inventory component
		nlohmann::json inventoryJson = nlohmann::json::array();
		int inventoryGroupId = characterComp.inventory.inventoryWorldId;

		WorldUtils::viewInSpecificLayer<ItemComponent>(
		    manager, inventoryGroupId,
		    [&manager, &characterComp, &inventoryGroupId, &inventoryJson](EntityID itemEntity,
		                                                                  ItemComponent &itemComp) {
			    nlohmann::json itemJson;
			    if (manager.hasComponent<PersistanceComponent>(itemEntity)) {
				    itemJson["uuid"] = manager.getComponent<PersistanceComponent>(itemEntity).uuid;
			    } else {
				    spdlog::error("Item {} has no PersistanceComponent! Cannot save.", itemComp.name);
				    return;
			    }
			    itemJson["itemType"] = static_cast<int>(itemComp.itemType);

			    bool isEquipped = (itemEntity.getId() == characterComp.equipedWeapon
			                       || itemEntity.getId() == characterComp.equipedCompanion);
			    itemJson["isEquipped"] = isEquipped;
			    itemJson["currentLayer"] = inventoryGroupId;
			    inventoryJson.push_back(itemJson);
		    });
		saveData["player"]["inventory"] = inventoryJson;

		if (manager.hasComponent<PartOfLayerComponent>(player)) {
			auto &layerComp = manager.getComponent<PartOfLayerComponent>(player);
			saveData["player"]["layerData"]["groupId"] = static_cast<int>(layerComp.groupId);
		}
	} else {
		spdlog::warn("SaveManager: No player found in ECS to save!");
	}
	// Save doors
	manager.view<PersistanceComponent, IsLockedComponent>().each(
	    [&saveData]([[maybe_unused]] EntityID id, const PersistanceComponent &persist, IsLockedComponent &lockComp) {
		    saveData["worldState"]["doorStates"][persist.uuid] = lockComp.isLocked;
	    });

	// Save dialogs
	manager.view<PersistanceComponent, DialogComponent, InteractionComponent>().each(
	    [&saveData]([[maybe_unused]] EntityID id, const PersistanceComponent &persist, DialogComponent &dialogComp,
	                InteractionComponent &interaction) {
		    saveData["worldState"]["dialogStates"][persist.uuid] = dialogComp.currentNodeIndex;
		    saveData["worldState"]["interactionStates"]["isActive"][persist.uuid] = interaction.isActive;
		    saveData["worldState"]["interactionStates"]["deactivated"][persist.uuid] = interaction.deactivated;
	    });

	auto currentMusicOpt = AudioManager::getInstance().getCurrentMusicName();
	if (currentMusicOpt.has_value()) {
		saveData["worldState"]["currentMusic"] = currentMusicOpt.value();
	} else {
		saveData["worldState"]["currentMusic"] = "";
	}

	saveData["worldState"]["deadUniqueEntities"] = PersistenceManager::getInstance().deadUniqueEntities;
	manager.view<WorldComponent>().each([&saveData]([[maybe_unused]] EntityID entity, WorldComponent &worldComp) {
		saveData["worldState"]["worldComponent"]["widthPixel"] = worldComp.widthPixel;
		saveData["worldState"]["worldComponent"]["heightPixel"] = worldComp.heightPixel;

		saveData["worldState"]["worldComponent"]["currentLevel"] = static_cast<int>(worldComp.currentLevel);
		saveData["worldState"]["worldComponent"]["currentLayer"] = static_cast<int>(worldComp.currentLayer);
		saveData["worldState"]["worldComponent"]["currentGroup"] = worldComp.currentGroup;

		saveData["worldState"]["worldComponent"]["menuOpened"] = worldComp.menuOpened;
	});

	std::string finalPath = getSaveFilePath(slotIndex);
	std::ofstream file(finalPath);
	if (!file.is_open()) {
		spdlog::error("OS blocked the game from creating the file at {}", finalPath);
		return;
	}
	file << saveData.dump(4);

	spdlog::info(getSaveFilePath(slotIndex));
	spdlog::info("Game successfully saved to Slot {}", slotIndex);
}

nlohmann::json SaveManager::loadSaveFile(int slotIndex)
{
	if (!doesSaveExist(slotIndex)) {
		throw std::runtime_error(fmt::format("Save slot {} does not exist!", slotIndex));
	}

	std::ifstream file(getSaveFilePath(slotIndex));
	nlohmann::json saveData;
	file >> saveData;

	PersistenceManager::getInstance().clearAll();

	if (saveData.contains("worldState")) {
		auto &worldState = saveData["worldState"];

		if (worldState.contains("deadUniqueEntities")) {
			PersistenceManager::getInstance().deadUniqueEntities =
			    worldState["deadUniqueEntities"].get<std::set<std::string>>();
		}
	}

	spdlog::info("Save data loaded into GameState from Slot {}", slotIndex);
	return saveData;
}

void SaveManager::applyWorldStateOverrides(ArchetypeManager &manager)
{
	const auto &deadEntities = PersistenceManager::getInstance().deadUniqueEntities;
	std::vector<EntityID> toDelete;

	manager.view<PersistanceComponent>().each(
	    [&deadEntities, &toDelete](EntityID id, const PersistanceComponent &persistent) {
		    if (deadEntities.find(persistent.uuid) != deadEntities.end()) {
			    toDelete.push_back(id);
			    return;
		    }
	    });

	for (EntityID id : toDelete) {
		manager.destroyEntity(id);
	}

	spdlog::info("World State Overrides Applied: Pruned {} dead entities.", toDelete.size());
}

void SaveManager::injectPlayer(ArchetypeManager &manager, const nlohmann::json &playerJson, EntityID &player)
{

	if (manager.hasComponent<TransformComponent>(player)) {
		auto &trans = manager.getComponent<TransformComponent>(player);
		trans.position.x = playerJson["position"]["x"];
		trans.position.y = playerJson["position"]["y"];
	}
	if (manager.hasComponent<CharacterComponent>(player)) {
		CharacterComponent &charComp = manager.getComponent<CharacterComponent>(player);
		charComp.stats.experience = playerJson["stats"].value("experience", 1.0f);
		charComp.stats.experienceLevel = playerJson["stats"].value("experienceLevel", 1);
		charComp.stats.numberOfFightsWon = playerJson["stats"].value("numberOfFightsWon", 0);
		charComp.stats.health = playerJson["stats"].value("health", 100);

		charComp.stats.addScalableStats(STRENGTH, playerJson["stats"].value("STRENGTH", 1));
		charComp.stats.addScalableStats(DEXTERITY, playerJson["stats"].value("DEXTERITY", 1));
		charComp.stats.addScalableStats(FAITH, playerJson["stats"].value("FAITH", 1));
		charComp.stats.addScalableStats(MAX_HEALTH, playerJson["stats"].value("MAX_HEALTH", 100));

		if (playerJson.contains("inventory")) {
			charComp.equipedWeapon = 0;
			charComp.equipedCompanion = 0;

			for (const auto &itemJson : playerJson["inventory"]) {
				if (!itemJson.contains("uuid"))
					continue;

				std::string savedUuid = itemJson["uuid"];
				ITEM_TYPE type = static_cast<ITEM_TYPE>(itemJson["itemType"].get<int>());
				bool isEquipped = itemJson.value("isEquipped", false);

				EntityID foundItem = EntityID();

				manager.view<PersistanceComponent, PartOfLayerComponent>().each(
				    [&foundItem, &savedUuid, &charComp](EntityID id, PersistanceComponent &persist,
				                                        PartOfLayerComponent &layerComp) {
					    if (persist.uuid == savedUuid) {
						    foundItem = id;
						    layerComp.groupId = charComp.inventory.inventoryWorldId;
					    }
				    });

				if (foundItem.getId() == 0) {
					spdlog::warn("Could not find mapped item with UUID {}!", savedUuid);
					continue;
				}
				if (isEquipped) {
					if (type == ITEM_TYPE::WEAPON) {
						charComp.equipedWeapon = foundItem.getId();
						spdlog::info("Equipped loaded weapon (New ID: {})", foundItem.getId());
					} else if (type == ITEM_TYPE::COLLECTABLE_COMPANION) {
						charComp.equipedCompanion = foundItem.getId();
						spdlog::info("Equipped loaded companion (New ID: {})", foundItem.getId());
					}
				}
			}
		}
	}
	if (playerJson.contains("layerData")) {
		auto &layerComp = manager.getComponent<PartOfLayerComponent>(player);
		layerComp.groupId = playerJson["layerData"].value("groupId", -1);
	}
	spdlog::info("Player successfully injected into the world.");
}

void SaveManager::injectWorldComponent(ArchetypeManager &manager, const nlohmann::json &worldStateJson)
{
	if (!worldStateJson.contains("worldComponent")) {
		return;
	}

	const auto &wcJson = worldStateJson["worldComponent"];

	manager.view<WorldComponent>().each([&wcJson]([[maybe_unused]] EntityID entity, WorldComponent &worldComp) {
		worldComp.widthPixel = wcJson.value("widthPixel", 0u);
		worldComp.heightPixel = wcJson.value("heightPixel", 0u);

		worldComp.currentLevel = static_cast<LEVEL_NAME>(wcJson.value("currentLevel", 0));
		worldComp.currentLayer = static_cast<LAYERTYPE>(wcJson.value("currentLayer", 0));
		worldComp.currentGroup = wcJson.value("currentGroup", -1);

		worldComp.menuOpened = wcJson.value("menuOpened", false);
	});
}

void SaveManager::injectDoors(ArchetypeManager &manager, const nlohmann::json &doorStates)
{
	manager.view<PersistanceComponent, IsLockedComponent, InteractionComponent>().each(
	    [&doorStates]([[maybe_unused]] EntityID id, const PersistanceComponent &persist, IsLockedComponent &lockComp,
	                  InteractionComponent &interactComp) {
		    if (doorStates.contains(persist.uuid)) {
			    bool isStillLocked = doorStates[persist.uuid];
			    lockComp.isLocked = isStillLocked;

			    if (!isStillLocked) {
				    interactComp.action = INTERACTION_ACTION::SWITCH_LAYER;
			    }
		    }
	    });
}

void SaveManager::injectDialogs(ArchetypeManager &manager, const nlohmann::json &dialogStates,
                                const nlohmann::json &interactionStates)
{
	manager.view<PersistanceComponent, DialogComponent, InteractionComponent>().each(
	    [&dialogStates, &interactionStates]([[maybe_unused]] EntityID id, const PersistanceComponent &persist,
	                                        DialogComponent &dialogComp, InteractionComponent &interactionComp) {
		    if (dialogStates.contains(persist.uuid)) {
			    dialogComp.currentNodeIndex = dialogStates[persist.uuid];
			    if (interactionStates.contains(persist.uuid)) {
				    interactionComp.isActive = interactionStates[persist.uuid].value("isActive", false);
				    interactionComp.deactivated = interactionStates[persist.uuid].value("deactivated", false);
			    }
			    if (dialogComp.isPassedOnce()) {
				    interactionComp.deactivated = true;
				    interactionComp.isActive = false;
			    }
		    }
	    });
}