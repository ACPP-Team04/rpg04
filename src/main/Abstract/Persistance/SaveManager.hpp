#pragma once

#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

class SaveManager {
  public:
	static void saveGame(ArchetypeManager &manager, int slotIndex);
	static nlohmann::json loadSaveFile(int slotIndex);
	static bool doesSaveExist(int slotIndex);

	static void applyWorldStateOverrides(ArchetypeManager &manager);

	static void injectPlayer(ArchetypeManager &manager, const nlohmann::json &playerJson);

  private:
	static std::string getSaveFilePath(int slotIndex);
};