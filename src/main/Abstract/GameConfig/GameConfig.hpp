#pragma once
#include <string>

class GameConfig {
  public:
	static GameConfig &getInstance();
	GameConfig(const GameConfig &) = delete;
	GameConfig &operator=(const GameConfig &) = delete;

	void loadConfig(const std::string &filepath);

	bool isGodModeEnabled() const { return godMode; }

  private:
	GameConfig() = default;

	bool godMode = false;
};