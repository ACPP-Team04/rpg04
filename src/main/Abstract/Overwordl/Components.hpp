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
		this->position.x = j.value("x", 0.0f);
		this->position.y = j.value("y", 0.0f);
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

struct WorldComponent : public Component<WorldComponent> {
	int tilewidth;
	int tileheight;
	int width;
	int height;
	unsigned widthPixel;
	unsigned heightPixel;
	std::vector<TileLayer> tileLayers;
	std::vector<ObjectLayer> objectLayers;

	void readFromJson(const nlohmann::json &j) override
	{
		tilewidth = j.value("tilewidth", 16);
		tileheight = j.value("tileheight", 16);
		width = j.value("width", 0);
		height = j.value("height", 0);
		widthPixel = width * tilewidth;
		heightPixel = height * tileheight;
		for (auto &layerJson : j["layers"]) {
			std::string type = layerJson.value("type", "");
			if (type == "tilelayer") {
				TileLayer layer;
				layer.readFromJson(layerJson, j);
				tileLayers.push_back(std::move(layer));
			} else if (type == "objectgroup") {
				ObjectLayer layer;
				layer.readFromJson(layerJson);
				objectLayers.push_back(std::move(layer));
			}
		}
	}
};