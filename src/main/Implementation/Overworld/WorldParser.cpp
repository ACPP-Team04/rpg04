#include "Abstract/Overwordl/WorldParser.hpp"

#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/Overwordl/Components/BoundingBoxComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/START_EQUIPMENT_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "tileson.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <fstream>
#include <utility>

WorldParser::WorldParser(ArchetypeManager &manager, sf::RenderWindow &window)
    : System(manager), window(window), project(PRO)
{
    tson::Tileson t{&project};
    map = t.parse(fs::path(MAP));
}


void WorldParser::undfoldLayers(std::vector<tson::Layer> & layers,std::vector<tson::Layer> & objectLayers, std::vector<tson::Layer> & tileLayers)
{
	for (auto & layer : layers) {
		if (layer.getType()==tson::LayerType::TileLayer) {
			tileLayers.push_back(layer);
		}
		if (layer.getType()==tson::LayerType::ObjectGroup) {
			objectLayers.push_back(layer);
		}
		this->undfoldLayers(layer.getLayers(), objectLayers, tileLayers);

	}
}

tson::Property getPropertyByPropertyType(tson::PropertyCollection props,std::string type)
{
	for (auto & prop : props.getProperties()) {
		if (prop.second.getPropertyType()==type) {
			return prop.second;
		}
	}
	throw std::logic_error(fmt::format("Unknown property type: {}", type));
};


tson::TiledClass getCustomPropertyAsClass(tson::Property prop)
{
	return prop.getValue<tson::TiledClass>();
}

void WorldParser::createEntity(std::tuple<tson::Object, LEVEL_NAME> & tuple)
{


	auto obj = std::get<0>(tuple);

	if (obj.getObjectType()!=tson::ObjectType::Rectangle && obj.getObjectType()!=tson::ObjectType::Object) {
		throw std::runtime_error("We allow only rectangle objects");
	}
	auto level = std::get<1>(tuple);
	EntityID id = manager.createEntityWithId(obj.getId());

	manager.addComponentToEntity<PartOfLayerComponent>(id);
	manager.getComponent<PartOfLayerComponent>(id).level = level;
	manager.addComponentToEntity<BoundIngBoxComponent>(id);

	manager.addComponentToEntity<TransformComponent>(id);
	manager.getComponent<TransformComponent>(id).position.x = (float)obj.getPosition().x;
	manager.getComponent<TransformComponent>(id).position.y = (float)obj.getPosition().y;
	manager.getComponent<TransformComponent>(id).setRotation((float)obj.getRotation());
	manager.getComponent<TransformComponent>(id).scale.x = (float)obj.getSize().x / (float)map->getTileSize().x;
	manager.getComponent<TransformComponent>(id).scale.y = (float)obj.getSize().y / (float)map->getTileSize().y;


	if (obj.isVisible()) {
		std::cout << obj.getId() << std::endl;
		manager.addComponentToEntity<RenderComponent>(id);
	}

	manager.addComponentToEntity<SpriteComponent>(id);
	auto &sprite = manager.getComponent<SpriteComponent>(id);

	if (obj.getObjectType()==tson::ObjectType::Object) {
		tson::Tileset *tileset = map->getTilesetByGid(obj.getGid());
		tson::Tile *tile = tileset->getTile(obj.getGid() - tileset->getFirstgid());
		tson::Rect drawingRect = tile->getDrawingRect();
		sprite.tileInfo = {drawingRect.x, drawingRect.y, drawingRect.width, drawingRect.height};
		sprite.tilesetPath = fs::path(MAP).parent_path() / tileset->getImagePath();

	}
	if (obj.getObjectType()==tson::ObjectType::Rectangle) {
		if (map->getTilesets().empty()) {
			throw std::runtime_error("You need to embedd a tileset!");
		}
		auto firsTileset = map->getTilesets()[0];

		int tilewidth = firsTileset.getTileSize().x;
		int tileheight = firsTileset.getTileSize().y;
		sprite.tileInfo = {0, 0, tilewidth, tileheight};
		sprite.tilesetPath = fs::path(MAP).parent_path() / firsTileset.getImagePath();

	}

	for (auto& prop : obj.getProperties().getProperties()) {
		std::string propType = prop.second.getPropertyType();
		std::cout << "Adding: " << propType << std::endl;
		ComponentRegistry &componentRegistry = ComponentRegistry::getInstance();
		auto func = componentRegistry.getCreationFunctions(propType);
		auto propClass = getCustomPropertyAsClass(prop.second);
		if (func) {

			func(manager, id, propClass);
		}
		std::cout << "Added:" << propType << std::endl;

	}


}
void WorldParser::update()
{
	tson::Tileson t;

	EntityID world = manager.createEntity<WorldComponent>();
	WorldComponent &worldComp = manager.getComponent<WorldComponent>(world);

	std::vector<tson::Layer> objectLayers;
	std::vector<tson::Layer> tileLayers;
	worldComp.widthPixel = map->getSize().x * map->getTileSize().x;
	worldComp.heightPixel = map->getSize().y * map->getTileSize().y;
	worldComp.currentLevel = LEVEL_NAME();
	worldComp.currentLayer = LAYERTYPE::OVERWORLD;
	window.setSize({worldComp.widthPixel,worldComp.heightPixel});

	sf::View view(sf::FloatRect({0.f, 0.f},
								{static_cast<float>(worldComp.widthPixel), static_cast<float>(worldComp.heightPixel)}));
	window.setView(view);


	undfoldLayers(map->getLayers(), objectLayers, tileLayers);

	std::map<int,std::tuple<tson::Object,LEVEL_NAME>> objects;

	for (auto & objlayer : objectLayers) {

		tson::Property config = getPropertyByPropertyType(objlayer.getProperties(), "LayerConfig");

		auto configObject = getCustomPropertyAsClass(config);
		int level_name2 = configObject.get<int>("layerType");
		LEVEL_NAME level_name = WorldUtils::getEnumValue<LEVEL_NAME>(configObject,"levelName");
		LAYERTYPE layertype = WorldUtils::getEnumValue<LAYERTYPE>(configObject,"layerType");
		for (auto & obj : objlayer.getObjects()) {
			objects[obj.getId()] = {obj,level_name};
		}
	}

	for (auto &tr: objects) {
		createEntity(tr.second);
	}


}