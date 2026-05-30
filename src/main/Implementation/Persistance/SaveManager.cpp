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

	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

	saveData["metadata"]["timestamp"] = ss.str();

	auto playerEntityOpt = WorldUtils::getPlayer(manager);
	if (playerEntityOpt.has_value()) {
		EntityID player = playerEntityOpt.value();

		// Save Position
		auto &trans = manager.getComponent<TransformComponent>(player);
		saveData["player"]["position"]["x"] = trans.position.x;
		saveData["player"]["position"]["y"] = trans.position.y;

		// Save Stats
		auto &stats = manager.getComponent<StatsComponent>(player);
		saveData["player"]["stats"]["experience"] = stats.experience;
		saveData["player"]["stats"]["experienceLevel"] = stats.experienceLevel;
		saveData["player"]["stats"]["numberOfFightsWon"] = stats.numberOfFightsWon;
		saveData["player"]["stats"]["health"] = stats.health;
		saveData["player"]["stats"]["STRENGTH"] = stats.getStat(STRENGTH);
		saveData["player"]["stats"]["DEXTERITY"] = stats.getStat(DEXTERITY);
		saveData["player"]["stats"]["FAITH"] = stats.getStat(FAITH);
		saveData["player"]["stats"]["MAX_HEALTH"] = stats.getStat(MAX_HEALTH);

		// Save your Inventory component
		auto &inventory = manager.getComponent<InventoryComponent>(player);
		nlohmann::json inventoryJson = nlohmann::json::array();

		for (const auto &[itemType, entitySet] : inventory.items) {

			for (EntityID itemEntity : entitySet) {
				nlohmann::json itemJson;

				itemJson["itemType"] = static_cast<int>(itemType);

				bool isEquipped = false;
				if (inventory.hasEquippedItem(itemType) && inventory.getEquippedItem(itemType) == itemEntity) {
					isEquipped = true;
				}
				itemJson["isEquipped"] = isEquipped;

				// Extract stats
				if (manager.hasComponent<WeaponComponent>(itemEntity)) {
					auto &weapon = manager.getComponent<WeaponComponent>(itemEntity);

					itemJson["weaponData"]["weaponType"] = static_cast<int>(weapon.weaponType);
					itemJson["weaponData"]["scalingFactor"] = static_cast<int>(weapon.scalingFactor);
					itemJson["weaponData"]["scalingStat"] = static_cast<int>(weapon.scalingStat);

					itemJson["weaponData"]["lightAttackBaseDmg"] = weapon.lightAttackBaseDmg;
					itemJson["weaponData"]["heavyAttackBaseDmg"] = weapon.heavyAttackBaseDmg;
					itemJson["weaponData"]["ultimateAttackBaseDmg"] = weapon.ultimateAttackBaseDmg;

					itemJson["weaponData"]["hitSoundLight"] = weapon.hitSoundLight;
					itemJson["weaponData"]["hitSoundHeavy"] = weapon.hitSoundHeavy;
					itemJson["weaponData"]["hitSoundUltimate"] = weapon.hitSoundUltimate;
				}
				inventoryJson.push_back(itemJson);
			}
		}

		saveData["player"]["inventory"] = inventoryJson;

		if (manager.hasComponent<PartOfLayerComponent>(player)) {
			auto &layerComp = manager.getComponent<PartOfLayerComponent>(player);

			saveData["player"]["layerData"]["level"] = static_cast<int>(layerComp.level);
			saveData["player"]["layerData"]["layer"] = static_cast<int>(layerComp.layer);
		}

	} else {
		spdlog::warn("SaveManager: No player found in ECS to save!");
	}

	saveData["worldState"]["deadUniqueEntities"] = PersistenceManager::getInstance().deadUniqueEntities;
	saveData["worldState"]["entityStates"] = PersistenceManager::getInstance().entityStates;

	manager.view<WorldComponent>().each([&](EntityID entity, WorldComponent &worldComp) {
		saveData["worldState"]["worldComponent"]["widthPixel"] = worldComp.widthPixel;
		saveData["worldState"]["worldComponent"]["heightPixel"] = worldComp.heightPixel;

		saveData["worldState"]["worldComponent"]["currentLevel"] = static_cast<int>(worldComp.currentLevel);
		saveData["worldState"]["worldComponent"]["currentLayer"] = static_cast<int>(worldComp.currentLayer);

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

		if (worldState.contains("entityStates")) {
			PersistenceManager::getInstance().entityStates =
			    worldState["entityStates"].get<std::map<std::string, int>>();
		}
	}

	spdlog::info("Save data loaded into GameState from Slot {}", slotIndex);
	return saveData;
}

void SaveManager::applyWorldStateOverrides(ArchetypeManager &manager)
{
	const auto &deadEntities = PersistenceManager::getInstance().deadUniqueEntities;
	const auto &savedStates = PersistenceManager::getInstance().entityStates;
	std::vector<EntityID> toDelete;

	manager.view<PersistanceComponent>().each([&](EntityID id, PersistanceComponent &persistent) {
		if (deadEntities.find(persistent.uuid) != deadEntities.end()) {
			toDelete.push_back(id);
			return;
		}

		// Did its state change?
		/*
		if (savedStates.find(persistent.uuid) != savedStates.end()) {
		    if (manager.hasComponent<InteractionComponent>(id)) {
		        manager.getComponent<InteractionComponent>(id).isActive = savedStates.at(persistent.uuid);
		    }
		}
		*/
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
	if (manager.hasComponent<TransformComponent>(player)) {
		auto &stats = manager.getComponent<StatsComponent>(player);
		stats.experience = playerJson["stats"].value("experience", 1.0f);
		stats.experienceLevel = playerJson["stats"].value("experienceLevel", 1);
		stats.numberOfFightsWon = playerJson["stats"].value("numberOfFightsWon", 0);
		stats.health = playerJson["stats"].value("health", 100);

		stats.addScalableStats(STRENGTH, playerJson["stats"].value("STRENGTH", 1));
		stats.addScalableStats(DEXTERITY, playerJson["stats"].value("DEXTERITY", 1));
		stats.addScalableStats(FAITH, playerJson["stats"].value("FAITH", 1));
		stats.addScalableStats(MAX_HEALTH, playerJson["stats"].value("MAX_HEALTH", 100));
	}
	if (manager.hasComponent<InventoryComponent>(player)) {
		auto &oldInv = manager.getComponent<InventoryComponent>(player);
		for (const auto &[type, entitySet] : oldInv.items) {
			for (EntityID id : entitySet) {
				manager.destroyEntity(id);
			}
		}
		manager.removeComponentFromEntity<InventoryComponent>(player);
	}
	manager.addComponentToEntity<InventoryComponent>(player);
	auto &inventory = manager.getComponent<InventoryComponent>(player);

	if (playerJson.contains("inventory")) {
		for (const auto &itemJson : playerJson["inventory"]) {
			EntityID newItem = manager.createEntity();
			ITEM_TYPE type = static_cast<ITEM_TYPE>(itemJson["itemType"].get<int>());

			if (itemJson.contains("weaponData")) {
				manager.addComponentToEntity<WeaponComponent>(newItem);
				auto &weapon = manager.getComponent<WeaponComponent>(newItem);
				const auto &wData = itemJson["weaponData"];

				weapon.weaponType = static_cast<WeaponType>(wData["weaponType"].get<int>());
				weapon.scalingFactor = static_cast<WEAPON_SCALING_FACTOR>(wData["scalingFactor"].get<int>());
				weapon.scalingStat = static_cast<STATS>(wData["scalingStat"].get<int>());

				weapon.lightAttackBaseDmg = wData["lightAttackBaseDmg"];
				weapon.heavyAttackBaseDmg = wData["heavyAttackBaseDmg"];
				weapon.ultimateAttackBaseDmg = wData["ultimateAttackBaseDmg"];

				weapon.hitSoundLight = wData["hitSoundLight"];
				weapon.hitSoundHeavy = wData["hitSoundHeavy"];
				weapon.hitSoundUltimate = wData["hitSoundUltimate"];
			}

			inventory.addItem(newItem, type);

			if (itemJson.value("isEquipped", false)) {
				inventory.equip(newItem, type);
			}
		}
	}
	if (playerJson.contains("layerData")) {
		auto &layerComp = manager.getComponent<PartOfLayerComponent>(player);

		layerComp.level = static_cast<LEVEL_NAME>(playerJson["layerData"].value("level", 0));
		layerComp.layer = static_cast<LAYERTYPE>(playerJson["layerData"].value("layer", 0));
	}
	spdlog::info("Player successfully injected into the world.");
}

void SaveManager::injectWorldComponent(ArchetypeManager &manager, const nlohmann::json &worldStateJson)
{
	if (!worldStateJson.contains("worldComponent")) {
		return;
	}

	const auto &wcJson = worldStateJson["worldComponent"];

	manager.view<WorldComponent>().each([&](EntityID entity, WorldComponent &worldComp) {
		worldComp.widthPixel = wcJson.value("widthPixel", 0u);
		worldComp.heightPixel = wcJson.value("heightPixel", 0u);

		worldComp.currentLevel = static_cast<LEVEL_NAME>(wcJson.value("currentLevel", 0));
		worldComp.currentLayer = static_cast<LAYERTYPE>(wcJson.value("currentLayer", 0));

		worldComp.menuOpened = wcJson.value("menuOpened", false);
	});
}