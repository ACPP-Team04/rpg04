#pragma once

#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

class SaveManager {
  public:
	static void saveGame(ArchetypeManager &manager, int slotIndex);
	static nlohmann::json loadSaveFile(int slotIndex);
	static bool doesSaveExist(int slotIndex);

	static void applyWorldStateOverrides(ArchetypeManager &manager);

	static void injectPlayer(ArchetypeManager &manager, const nlohmann::json &playerJson, const EntityID &playerID);
	static void injectWorldComponent(ArchetypeManager &manager, const nlohmann::json &worldStateJson);
	static void injectDoors(ArchetypeManager &manager, const nlohmann::json &doorStates);
	static void injectDialogs(ArchetypeManager &manager, const nlohmann::json &dialogStates,
	                          const nlohmann::json &interactionStates);
	static void injectInventory(ArchetypeManager &manager, CharacterComponent &charComp,
	                            const nlohmann::json &inventoryJson);
	static void deleteSave(int slotIndex);

  private:
	static std::string getSaveFilePath(int slotIndex);
};