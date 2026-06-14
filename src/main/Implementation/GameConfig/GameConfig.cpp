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
	bool fileLoadedSuccessfully = false;
	std::string parseErrorMessage = "";

	if (configFile.is_open()) {
		try {
			configFile >> m_data;
			fileLoadedSuccessfully = true;
		} catch (const nlohmann::json::parse_error &e) {
			parseErrorMessage = e.what();
		}
	} else {
	}

	if (m_data.contains("debug")) {
		godMode = m_data["debug"].value("godMode", false);
	} else {
		godMode = false;
	}
	logLevel = m_data.value("log_level", "info");
	spdlog::set_level(spdlog::level::from_str(logLevel));
	if (fileLoadedSuccessfully) {
		spdlog::info("GameConfig: Successfully loaded config file at {}", filepath);
	} else if (!parseErrorMessage.empty()) {
		spdlog::error("GameConfig: Syntax error in config file. Using default values. Error: {}", parseErrorMessage);
	} else {
		spdlog::warn("GameConfig: Could not find config file at {}. Using default values.", filepath);
	}
}