#pragma once
#include <map>
#include <set>
#include <string>

class GameState {
  public:
	static GameState &getInstance()
	{
		static GameState instance;
		return instance;
	}

	std::set<std::string> deadUniqueEntities;

	std::map<std::string, int> entityStates;
	bool requestLoad = false;

	void clearAll()
	{
		deadUniqueEntities.clear();
		entityStates.clear();
	}

	GameState(const GameState &) = delete;
	GameState &operator=(const GameState &) = delete;

  private:
	GameState() = default;
};