#include "Abstract/Persistance/SaveManager.hpp"
#include "Abstract/GameState/GameState.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/PersistanceComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Overwordl/Components/TransformComponent.hpp>

#include <fstream>
#include <spdlog/spdlog.h>

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

		// TODO: Save your Inventory component
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

				// 3. Extract the actual stats from the ECS based on what the item is
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

	} else {
		spdlog::warn("SaveManager: No player found in ECS to save!");
	}

	saveData["worldState"]["deadUniqueEntities"] = GameState::getInstance().deadUniqueEntities;
	saveData["worldState"]["entityStates"] = GameState::getInstance().entityStates;

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

	GameState::getInstance().clearAll();

	if (saveData.contains("worldState")) {
		auto &worldState = saveData["worldState"];

		if (worldState.contains("deadUniqueEntities")) {
			GameState::getInstance().deadUniqueEntities = worldState["deadUniqueEntities"].get<std::set<std::string>>();
		}

		if (worldState.contains("entityStates")) {
			GameState::getInstance().entityStates = worldState["entityStates"].get<std::map<std::string, int>>();
		}
	}

	spdlog::info("Save data loaded into GameState from Slot {}", slotIndex);
	return saveData;
}

void SaveManager::applyWorldStateOverrides(ArchetypeManager &manager)
{
	const auto &deadEntities = GameState::getInstance().deadUniqueEntities;
	const auto &savedStates = GameState::getInstance().entityStates;
	std::vector<EntityID> toDelete;

	manager.view<PersistanceComponent>().each([&](EntityID id, PersistanceComponent &persistent) {
		if (deadEntities.find(persistent.uuid) != deadEntities.end()) {
			toDelete.push_back(id);
			return;
		}

		// RULE 2: Did its state change?
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

void SaveManager::injectPlayer(ArchetypeManager &manager, const nlohmann::json &playerJson)
{
	EntityID player = manager.createEntity();
	manager.addComponentToEntity<PlayerComponent>(player);

	manager.addComponentToEntity<TransformComponent>(player);
	auto &trans = manager.getComponent<TransformComponent>(player);
	trans.position.x = playerJson["position"]["x"];
	trans.position.y = playerJson["position"]["y"];

	manager.addComponentToEntity<StatsComponent>(player);
	auto &stats = manager.getComponent<StatsComponent>(player);
	stats.experience = playerJson["player"]["stats"]["experience"];
	stats.experienceLevel = playerJson["player"]["stats"]["experienceLevel"];
	stats.numberOfFightsWon = playerJson["player"]["stats"]["numberOfFightsWon"];
	stats.health = playerJson["player"]["stats"]["health"];

	stats.addScalableStats(STRENGTH, playerJson["player"]["stats"]["STRENGTH"]);
	stats.addScalableStats(DEXTERITY, playerJson["player"]["stats"]["DEXTERITY"]);
	stats.addScalableStats(FAITH, playerJson["player"]["stats"]["FAITH"]);
	stats.addScalableStats(MAX_HEALTH, playerJson["player"]["stats"]["MAX_HEALTH"]);

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
	spdlog::info("Player successfully injected into the world.");
}