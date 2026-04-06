#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/Color.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct TransformComponent : public Component<TransformComponent> {
	sf::Vector2f position;
	sf::Vector2f scale{1.0f, 1.0f};
	sf::Angle rotation = sf::Angle::Zero;
	sf::Vector2f previousPosition;

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
	bool justPressed = false;
};

struct InputComponent : public Component<InputComponent> {
	KeyState moveUp, moveDown, moveRight, moveLeft, menuButton, interact;

	void readFromJson(const nlohmann::json &j) override
	{
		this->moveUp = KeyState(j.value("up", false));
		this->moveDown = KeyState(j.value("down", false));
		this->moveRight = KeyState(j.value("right", false));
		this->moveLeft = KeyState(j.value("left", false));
		this->menuButton = KeyState(j.value("button", false));
		this->interact = KeyState(j.value("interact", false));
	}
};

struct SpriteComponent : public Component<SpriteComponent> {
	TileType textureId;

	void readFromJson(const nlohmann::json &j) override
	{
		this->textureId = static_cast<TileType>(j.value("texture", 0));
	}
};

struct RenderComponent : public Component<RenderComponent> {

	int z_layer;
	void readFromJson(const nlohmann::json &j) override {}
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

struct NPC_Component : public Component<NPC_Component> {

	void readFromJson(const nlohmann::json &j) override
	{

	}
};

struct PlayerComponent : public Component<PlayerComponent> {

	void readFromJson(const nlohmann::json &j) override
	{

	}
};

struct DialogComponent : public Component<DialogComponent> {
	std::vector<std::string> sentences;
	sf::Keyboard::Key startsWith;
	sf::Color color;
	int characterSize;
	std::string currentSentence;
	int senetnceId;
	bool isActive;
	void readFromJson(const nlohmann::json &j) override
	{
		std::string raw = j.value("dialogSentences", "[]");
		auto parsed = nlohmann::json::parse(raw);
		for (const auto &sentence : parsed) {
			sentences.push_back(sentence.get<std::string>());
		}
		startsWith = static_cast<sf::Keyboard::Key>(j.value("startsWithButton", 0));
		color = parseColorString(j.value("fillColor", "#f7f7f7"));
		characterSize = j.value("characterSize",10);
		currentSentence = this->sentences.back();
	}

	std::string getKeyAsString(sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Key::F) {
			return "F";
		}
		else return "G";
	}

	std::string getSentence()
	{
		return currentSentence;
	}

	void nextSentence()
	{
		int newSentence = senetnceId%sentences.size();
		currentSentence = sentences[newSentence];
		senetnceId++;
	}
};

struct InteractionComponent : public Component<InteractionComponent> {

	bool isActive;

	void readFromJson(const nlohmann::json &j) override
	{
		isActive = false;
	}
};

struct SwitchLayerComponent : public Component<SwitchLayerComponent> {
	LEVEL_NAME level;
	LAYERTYPE layer;
	void readFromJson(const nlohmann::json &j) override
	{
		level = (LEVEL_NAME)j.value("level", 0);
		layer = (LAYERTYPE)j.value("layer", 0);
	}
};

struct PartOfLayerComponent : public Component<PartOfLayerComponent> {
	LEVEL_NAME level;
	LAYERTYPE layer;
	void readFromJson(const nlohmann::json &j) override {}
};

struct CollisionComponent : public Component<CollisionComponent> {
	COLLISION_ACTION action;
	bool isStatic;
	void readFromJson(const nlohmann::json &j) override
	{
		action = (COLLISION_ACTION)j.value("action", 0);
		isStatic = j.value("isStatic", false);
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