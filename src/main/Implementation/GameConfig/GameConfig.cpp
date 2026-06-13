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

	if (configFile.is_open()) {
		try {
			configFile >> m_data;
			spdlog::info("GameConfig: Successfully loaded config file at {}", filepath);
		} catch (const nlohmann::json::parse_error &e) {
			spdlog::error("GameConfig: Syntax error in config file. Using default values. Error: {}", e.what());
			return;
		}
	} else {
		spdlog::warn("GameConfig: Could not find config file at {}. Using default values.", filepath);
		return;
	}

	if (m_data.contains("debug")) {
		godMode = m_data["debug"].value("godMode", false);
	}
}