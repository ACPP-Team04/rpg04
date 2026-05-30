#pragma once
#include <map>
#include <set>
#include <string>

class PersistenceManager {
  public:
	static PersistenceManager &getInstance()
	{
		static PersistenceManager instance;
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

	PersistenceManager(const PersistenceManager &) = delete;
	PersistenceManager &operator=(const PersistenceManager &) = delete;

  private:
	PersistenceManager() = default;
};