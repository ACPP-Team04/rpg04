#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/System/Vector2.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>


struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;

	void readFromJson(const nlohmann::json &j) override
	{

		this->position.x = j.value("position_x", 0.0f);
		this->position.y = j.value("position_y", 0.0f);
		this->scale.x = j.value("scale_x", 1.0f);
		this->scale.y = j.value("scale_y", 1.0f);
		this->rotation = sf::degrees(j.value("rotation", 0.0f));
	}


};

struct KeyState {
	bool pressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton;

	void readFromJson(const nlohmann::json &j) override
	{
		this->moveUp = KeyState(j.value("up", false));
		this->moveDown = KeyState(j.value("down", false));
		this->moveRight = KeyState(j.value("right", false));
		this->moveLeft = KeyState(j.value("left", false));
		this->menuButton = KeyState(j.value("button", false));
	}
};

struct RenderComponent : public Component<RenderComponent> {
	std::string activeTiles;
	void readFromJson(const nlohmann::json &j) override { this->activeTiles = j.value("activeTile", "HOUSE1_BROWN"); }
};

struct MovementComponent : public Component<MovementComponent> {
	float speed;

	void readFromJson(const nlohmann::json &j) override { this->speed = j.value("speed", 1.0f); };
};

struct CameraComponent : public Component<CameraComponent> {
	sf::Vector2f center;
	sf::Vector2f scaleSize;

	void readFromJson(const nlohmann::json &j) override
	{
		scaleSize.x = j.value("scale_x", 1.0f);
		scaleSize.y = j.value("scale_y", 1.0f);
	}
};

struct CurrentLayerComponent : public Component<CurrentLayerComponent> {
	int level;
	std::string layer;
	void readFromJson(const nlohmann::json &j) override
	{
		level = j.value("level", 0);
		layer = j.value("layer", "overworld");
	}
};

struct SwitchLayerComponent : public Component<SwitchLayerComponent> {
	int level;
	std::string layer;
	void readFromJson(const nlohmann::json &j) override
	{
		level = j.value("level", 0);
		layer = j.value("layer", "overworld");
	}
};

struct PartOfLayerComponent : public Component<PartOfLayerComponent> {
	int level;
	std::string layer;
	void readFromJson(const nlohmann::json &j) override
	{
	}
};

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
struct OverworldLayer :WorldLayer {
};

struct BattleLayer:WorldLayer {
};

struct LevelLayer {
	std::vector<OverworldLayer> overworld_layer;
	std::vector<BattleLayer> battle_layer;
};


static std::string OVERWORLD_LAYER = "overworld";
static std::string BATTLE_LAYER = "battlelayer";
static std::string TILE_LAYER = "tilelayer";
static std::string OBJECT_LAYER = "objectgroup";
static std::string LEVEL_LAYER_PREFIX = "level";
struct WorldComponent : public Component<WorldComponent> {
	int tilewidth;
	int tileheight;
	int width;
	int height;
	unsigned widthPixel;
	unsigned heightPixel;
	std::vector<LevelLayer> levelLayers;

	void readFromJson(const nlohmann::json &j) override
	{
		tilewidth = j.value("tilewidth", 16);
		tileheight = j.value("tileheight", 16);
		width = j.value("width", 0);
		height = j.value("height", 0);
		widthPixel = width * tilewidth;
		heightPixel = height * tileheight;
		unfoldLayers(j);
	}



	void unfoldLayers(const nlohmann::json &j)
	{
		for (const auto &layer : j.value("layers", nlohmann::json::array())) {
			if (layer.value("type", "") != "group") {
				continue;
			}
			auto levelName = layer.value("name", "");
			if (!levelName.starts_with(LEVEL_LAYER_PREFIX)){
				std::cout << levelName << std::endl;
				throw std::runtime_error(
					"Invalid layer type. Group layer has to start with Level");
			}

			LevelLayer levelLayer;
			auto sublayers = layer.value("layers", nlohmann::json::array());
			if (sublayers.size() != 2) {
				throw std::runtime_error(

					"Invalid layer type. Each level needs exactly one BattleLayer and one OverworldLayer");
			}
			for (const auto &sublayer : sublayers) {
				std::string name = sublayer.value("name", "");
				if (name == BATTLE_LAYER) {
					BattleLayer battleLayer;
					for (const auto &l : sublayer.value("layers", nlohmann::json::array())) {
						setLayer(l, battleLayer, j);
					}
					levelLayer.battle_layer.push_back(std::move(battleLayer));

				}
				else if (name == OVERWORLD_LAYER) {
					OverworldLayer overworldLayer;
					for (const auto &l : sublayer.value("layers", nlohmann::json::array())) {
						setLayer(l, overworldLayer, j);
					}
					levelLayer.overworld_layer.push_back(std::move(overworldLayer));
				}
			}

			levelLayers.push_back(std::move(levelLayer));
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