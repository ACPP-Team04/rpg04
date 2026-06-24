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
	bool fileLoadedSuccessfully = false;
	std::string parseErrorMessage = "";

	if (std::ifstream configFile(filepath); configFile.is_open()) {
		try {
			configFile >> m_data;
			fileLoadedSuccessfully = true;
		} catch (const nlohmann::json::parse_error &e) {
			parseErrorMessage = e.what();
		}
	} else {
		spdlog::warn("GameConfig: Could not open config file at {}. Using default values.", filepath);
	}

	if (m_data.contains("debug")) {
		godMode = m_data["debug"].value("godMode", false);
	} else {
		godMode = false;
	}
	logLevel = m_data.value("log_level", "info");
	defaultMusic = m_data.value("default_music", "");
	spdlog::set_level(spdlog::level::from_str(logLevel));
	if (fileLoadedSuccessfully) {
		spdlog::info("GameConfig: Successfully loaded config file at {}", filepath);
	} else if (!parseErrorMessage.empty()) {
		spdlog::error("GameConfig: Syntax error in config file. Using default values. Error: {}", parseErrorMessage);
	} else {
		spdlog::warn("GameConfig: Could not find config file at {}. Using default values.", filepath);
	}
}