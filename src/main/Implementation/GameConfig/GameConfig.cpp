#include "Abstract/GameConfig/GameConfig.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

GameConfig &GameConfig::getInstance()
{
	static GameConfig instance;
	return instance;
}

void GameConfig::loadConfig(const std::string &filepath)
{
	std::ifstream configFile(filepath);
	nlohmann::json configJson;

	if (configFile.is_open()) {
		try {
			configFile >> configJson;
			spdlog::info("GameConfig: Successfully loaded config file at {}", filepath);
		} catch (const nlohmann::json::parse_error &e) {
			spdlog::error("GameConfig: Syntax error in config file. Using default values. Error: {}", e.what());
			return;
		}
	} else {
		spdlog::warn("GameConfig: Could not find config file at {}. Using default values.", filepath);
		return;
	}

	if (configJson.contains("debug")) {
		godMode = configJson["debug"].value("godMode", false);
	}
}