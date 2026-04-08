#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/Color.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct tileProperty {
	std::string name;
	std::string type;
	nlohmann::json value;
	std::string propertytype;

	void readFromJson(const nlohmann::json &data)
	{
		name = data.value("name", "");
		type = data.value("type", "");
		value = data.value("value", nlohmann::json());
		propertytype = data.value("propertytype", "");
	}
};

template <typename T>
void readJsonPropertyArray(std::vector<T> &element, const nlohmann::json &j, std::string key)
{

	auto rawElement = j.value(key, nlohmann::json::array());
	for (auto &i : rawElement) {
		T elem;
		elem.readFromJson(i);
		element.push_back(std::move(elem));
	}
}

struct Tile {
	int tile;
	std::vector<tileProperty> properties;

	void readFromJson(const nlohmann::json &j)
	{
		tile = j.value("tile", 0);
		readJsonPropertyArray<tileProperty>(properties, j, "properties");
	}
};
struct TileLayer {
	int height = 0;
	int width = 0;
	std::vector<Tile> tileIds;

	void readFromJson(const nlohmann::json &j, const nlohmann::json &data)
	{
		height = j.value("height", 0);
		width = j.value("width", 0);
		std::vector<int> tileIdsInt = j["data"].get<std::vector<int>>();
		tileIds.reserve(tileIdsInt.size());

		for (auto &gid : tileIdsInt) {
			Tile currentTile;
			currentTile.tile = gid;
			if (gid != 0) {
				for (const auto &tileset : data.value("tilesets", nlohmann::json::array())) {
					int firstgid = 1;
					for (const auto &tileData : tileset.value("tiles", nlohmann::json::array())) {
						if (tileData.value("id", 0) == (gid - firstgid)) {
							readJsonPropertyArray<tileProperty>(currentTile.properties, tileData, "properties");
						}
					}
				}
			}
			tileIds.push_back(std::move(currentTile));
		}
	}
};

struct ObjectLayerObject {
	int id;
	int x, y;
	int height;
	int width;
	std::vector<tileProperty> properties;

	void readFromJson(const nlohmann::json &j)
	{
		id = j.value("id", 0);
		x = j.value("x", 0.0f);
		y = j.value("y", 0.0f);
		height = j.value("height", 0);
		width = j.value("width", 0);
		readJsonPropertyArray<tileProperty>(properties, j, "properties");
	}
};

struct ObjectLayer {

	std::string name;
	std::vector<ObjectLayerObject> objects;
	void readFromJson(const nlohmann::json &j)
	{
		name = j.value("name", "Object1");
		readJsonPropertyArray<ObjectLayerObject>(objects, j, "objects");
	}
};

struct WorldLayer {
	std::vector<TileLayer> tileLayers;
	std::vector<ObjectLayer> objectLayers;
};
struct LevelLayer {
	std::unordered_map<LAYERTYPE, WorldLayer> layers;
};

static std::string TILE_LAYER = "tilelayer";
static std::string OBJECT_LAYER = "objectgroup";

struct WorldComponent : public Component<WorldComponent> {
	int tilewidth;
	int tileheight;
	int width;
	int height;
	unsigned widthPixel;
	unsigned heightPixel;
	std::unordered_map<LEVEL_NAME, LevelLayer> levelLayers;
	LEVEL_NAME currentLevel = (LEVEL_NAME)0;
	LAYERTYPE currentLayer = (LAYERTYPE)0;
	std::unordered_map<LAYERTYPE, MENUS> menus;
	bool menuOpened = false;

	void readFromJson(const nlohmann::json &j) override
	{
		tilewidth = j.value("tilewidth", 16);
		tileheight = j.value("tileheight", 16);
		width = j.value("width", 0);
		height = j.value("height", 0);
		widthPixel = width * tilewidth;
		heightPixel = height * tileheight;
		unfoldLayers(j, j);
	}

	template <typename T>
	static T parseLevelConfig(std::vector<tileProperty> &properties, const std::string &key)
	{
		for (const auto &prop : properties) {
			if (prop.propertytype == "LayerConfig") {
				return (T)prop.value.value(key, 0);
			}
		}
		throw std::runtime_error("LayerConfig property not found for key: " + key);
	}

	void register_menu(LAYERTYPE layer, MENUS menu) { this->menus.insert(std::make_pair(layer, menu)); }

	void unfoldLayers(const nlohmann::json &j, const nlohmann::json &rootJson)
	{
		for (const auto &layer : j.value("layers", nlohmann::json::array())) {
			if (layer.value("type", "") != OBJECT_LAYER && layer.value("type", "") != TILE_LAYER) {
				unfoldLayers(layer, rootJson);
				continue;
			}
			std::vector<tileProperty> properties;
			readJsonPropertyArray(properties, layer, "properties");
			if (properties.empty()) {
				// TODO  create here a log warning
				continue;
			}
			auto layerName = parseLevelConfig<LEVEL_NAME>(properties, "levelName");
			auto layerType = parseLevelConfig<LAYERTYPE>(properties, "layerType");

			WorldLayer worldLayer;
			setLayer(layer, worldLayer, rootJson);
			setLayer(layer, levelLayers[layerName].layers[layerType], rootJson);
		}
	}
	void setLayer(const nlohmann::json &layerJson, WorldLayer &worldLayer, const nlohmann::json &rootJson)
	{
		std::string type = layerJson.value("type", "");
		if (type == TILE_LAYER) {
			std::cout << TILE_LAYER << std::endl;
			TileLayer layer;
			layer.readFromJson(layerJson, rootJson);
			worldLayer.tileLayers.push_back(std::move(layer));
		} else if (type == OBJECT_LAYER) {
			ObjectLayer layer;
			layer.readFromJson(layerJson);
			worldLayer.objectLayers.push_back(std::move(layer));
		}
	}
};