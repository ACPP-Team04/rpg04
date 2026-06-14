#pragma once
#include <nlohmann/json.hpp>
#include <string>

class GameConfig {
  public:
	static GameConfig &getInstance();
	GameConfig(const GameConfig &) = delete;
	GameConfig &operator=(const GameConfig &) = delete;

	void loadConfig(const std::string &filepath);

	bool isGodModeEnabled() const { return godMode; }

	std::unordered_map<int, std::string> getMusicMappings()
	{
		std::unordered_map<int, std::string> map;
		auto &json = m_data["music_mappings"];
		for (auto it = json.begin(); it != json.end(); ++it) {
			map[std::stoi(it.key())] = it.value().get<std::string>();
		}
		return map;
	}

	std::string getLogLevel() const { return logLevel; }

  private:
	GameConfig() = default;

	bool godMode = false;
	std::string logLevel = "info";
	nlohmann::json m_data;
};