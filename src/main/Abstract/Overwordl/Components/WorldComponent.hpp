#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "SpriteComponent.hpp"
#include "tileson.h"
#include <SFML/System/Vector2.hpp>

#include <nlohmann/json.hpp>
#include <queue>
#include <unordered_map>

enum class ACTIVE_MENU { NONE, PAUSE_MENU, INVENTORY_MENU };

struct Tile {
	TileInfo tileInfo;
	sf::Vector2f position;
};
struct TileLayer {

	std::vector<std::vector<Tile>> tiles;

	void init(int width, int height) { tiles.assign(height, std::vector<Tile>(width)); }
};

struct World {
	std::string name = "WORLD_WITH_NO_NAME";
	int id;
	std::vector<TileLayer> tileLayers;
	std::vector<TileLayer> collidingTileLayers;
};
struct WorldComponent : public Component<WorldComponent> {
	unsigned width = 0;
	unsigned height = 0;
	unsigned widthPixel = 0;
	unsigned heightPixel = 0;
	unsigned tileWidth = 0;
	unsigned tileHeight = 0;

	std::unordered_map<int, World> worlds = {};
	int currentGroup;

	LEVEL_NAME currentLevel = (LEVEL_NAME)0;
	LAYERTYPE currentLayer = (LAYERTYPE)0;
	std::unordered_map<LAYERTYPE, MENUS> menus;

	std::queue<std::string> toast;
	std::unordered_set<std::string> persistentMessages;

	bool menuOpened = false;
	ACTIVE_MENU currentMenu = ACTIVE_MENU::NONE;

	void readFromJson(tson::TiledClass &j) override {};

	void readFromJson(const std::unique_ptr<tson::Map> &map, nlohmann::json &rawJson)
	{
		tileWidth = map->getTileSize().x;
		tileHeight = map->getTileSize().y;
		width = map->getSize().x;
		height = map->getSize().y;
		widthPixel = width * tileWidth;
		heightPixel = height * tileHeight;

		readLayersFromJson(rawJson["layers"], rawJson["tilesets"], -1, "");
	}

	void readLayersFromJson(nlohmann::json &layers, nlohmann::json &tilesets, int groupId, std::string groupName)
	{
		for (auto &layer : layers) {
			if (layer["type"] == "group") {
				int id = layer["id"].get<int>();
				worlds[id] = {layer["name"].get<std::string>(), id};
				readLayersFromJson(layer["layers"], tilesets, id, layer["name"].get<std::string>());
			} else if (layer["type"] == "tilelayer") {
				TileLayer tileLayer;
				tileLayer.init(width, height);
				bool collides = false;
				auto &data = layer["data"];
				bool hasProperties = layer.contains("properties") && layer["properties"].is_array() && !layer["properties"].empty();
				if (hasProperties) {
					for (auto &prop : layer["properties"]) {
						if (prop["name"] == "collision" && prop["value"] == true) {
							collides = true;
							break;
						}
					}
				}

				for (int i = 0; i < (int)data.size(); i++) {
					int gid = data[i].get<int>();
					if (gid == 0)
						continue;
					int x = i % width;
					int y = i / width;
					for (auto &ts : tilesets) {
						if (!ts.contains("columns"))
							continue;
						int firstgid = ts["firstgid"].get<int>();
						int count = ts["tilecount"].get<int>();
						if (gid < firstgid || gid >= firstgid + count)
							continue;
						int localId = gid - firstgid;
						int cols = ts["columns"].get<int>();
						int tw = ts["tilewidth"].get<int>();
						int th = ts["tileheight"].get<int>();
						tileLayer.tiles[y][x].tileInfo.pixelX = (localId % cols) * tw;
						tileLayer.tiles[y][x].tileInfo.pixelY = (localId / cols) * th;
						tileLayer.tiles[y][x].tileInfo.width = tw;
						tileLayer.tiles[y][x].tileInfo.height = th;
						tileLayer.tiles[y][x].tileInfo.tilesetPath =
						    (fs::path(MAP).parent_path() / ts["image"].get<std::string>()).string();
						tileLayer.tiles[y][x].position = {(float)(x * tileWidth), (float)(y * tileHeight)};
						break;
					}
				}
				if (!collides) {
					worlds[groupId].tileLayers.push_back(tileLayer);
					continue;
				}
				worlds[groupId].collidingTileLayers.push_back(tileLayer);

			}
		}
	}

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