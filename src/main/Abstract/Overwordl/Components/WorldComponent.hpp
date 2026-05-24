#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <queue>
#include <unordered_map>

struct WorldComponent : public Component<WorldComponent> {
	unsigned widthPixel;
	unsigned heightPixel;

	LEVEL_NAME currentLevel = (LEVEL_NAME)0;
	LAYERTYPE currentLayer = (LAYERTYPE)0;
	std::unordered_map<LAYERTYPE, MENUS> menus;

	std::queue<std::string> toast;
	std::unordered_set<std::string> persistentMessages;

	bool menuOpened = false;

	void readFromJson(tson::TiledClass &j) {}

	void pushMessageToHud(std::string text) { this->toast.push(text); }

	bool hasMessageInHud() { return this->toast.size() > 0; }

	void addPersistentMessage(std::string text) { this->persistentMessages.insert(text); }

	void removePersistentMessage(std::string text)
	{
		if (!this->persistentMessages.contains(text)) {
			return;
		}
		this->persistentMessages.erase(text);
	}

	std::string readMessageFromHud()
	{
		std::string message = this->toast.front();
		this->toast.pop();
		return message;
	}
};