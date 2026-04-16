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

void equibFists(ArchetypeManager &manager, const WorldComponent &component)
{
	manager.view<InventoryComponent>().each([&](auto &entity, InventoryComponent &icomp) {
		if (!icomp.hasEquippedItem(ITEM_TYPE::WEAPON)) {
			EntityID weapon = manager.createEntity<ItemComponent, WeaponComponent>();
			manager.getComponent<ItemComponent>(weapon).itemType = ITEM_TYPE::WEAPON;
			icomp.addItem(weapon, ITEM_TYPE::WEAPON);
			
		}
		if (!manager.hasComponent<StatsComponent>(entity)) {
				manager.addComponentToEntity<StatsComponent>(entity);
				std::cout << "Added stats component to entity: " << entity.getId() << std::endl;
				std::cout <<manager.getComponent<StatsComponent>(entity).health<< std::endl;
			}
	});
}
void parseRawEquipmentComponent(ArchetypeManager &manager, const WorldComponent &component)
{
	std::vector<EntityID> entityIds;
	manager.view<InventoryComponent, START_EQUIPMENT_COMPONENT>().each(
	    [&](auto &entity, InventoryComponent &icomp, START_EQUIPMENT_COMPONENT &eqipComp) {
		    entityIds.push_back(entity.getId());
		    EntityID weapon = manager.createEntity<ItemComponent, WeaponComponent>();
		    manager.getComponent<ItemComponent>(weapon).itemType = ITEM_TYPE::WEAPON;
		    manager.getComponent<WeaponComponent>(weapon).readFromJson(eqipComp.rawWeaponComponent);
		    EntityID healing = manager.createEntity<ItemComponent, ITEM_HEALSTATS_COMPONENT>();
		    manager.getComponent<ItemComponent>(healing).itemType = ITEM_TYPE::HEALING;
		    manager.getComponent<ITEM_HEALSTATS_COMPONENT>(healing).healAmount = eqipComp.healing;

		    icomp.addItem(weapon, ITEM_TYPE::WEAPON);
		    icomp.addItem(healing, ITEM_TYPE::HEALING);
	    });

	for (const auto &entity : entityIds) {
		manager.removeComponentFromEntity<START_EQUIPMENT_COMPONENT>(entity);
	}
}

void WorldParser::addTransformcomponent(ArchetypeManager &manager, EntityID id, tson::Object obj,LEVEL_NAME level)
{
	manager.addComponentToEntity<PartOfLayerComponent>(id);
	manager.getComponent<PartOfLayerComponent>(id).level = level;
	manager.addComponentToEntity<BoundIngBoxComponent>(id);

	manager.addComponentToEntity<TransformComponent>(id);
	manager.getComponent<TransformComponent>(id).position.x = (float)obj.getPosition().x;
	manager.getComponent<TransformComponent>(id).position.y = (float)obj.getPosition().y;
	manager.getComponent<TransformComponent>(id).setRotation((float)obj.getRotation());
	float sx = (float)obj.getSize().x / (float)map->getTileSize().x;
	float sy = (float)obj.getSize().y / (float)map->getTileSize().y;
	manager.getComponent<TransformComponent>(id).scale.x = (sx > 0.f) ? sx : 1.f;
	manager.getComponent<TransformComponent>(id).scale.y = (sy > 0.f) ? sy : 1.f;
}

void WorldParser::addRenderComponent(ArchetypeManager &manager, EntityID id, tson::Object obj)
{
	if (obj.isVisible()) {
		std::cout << obj.getId() << std::endl;
		manager.addComponentToEntity<RenderComponent>(id);
		manager.getComponent<RenderComponent>(id).z_layer = 1;
	}
}

void WorldParser::addSpriteComponent(ArchetypeManager &manager, EntityID id, tson::Object obj)
{
	manager.addComponentToEntity<SpriteComponent>(id);
	auto &sprite = manager.getComponent<SpriteComponent>(id);

	if (obj.getObjectType() == tson::ObjectType::Object) {
		tson::Tileset *tileset = map->getTilesetByGid(obj.getGid());
		int localId = obj.getGid() - tileset->getFirstgid();
		int cols    = tileset->getColumns();
		int tw      = tileset->getTileSize().x;
		int th      = tileset->getTileSize().y;
	
		sprite.tileInfo = {
			(localId % cols) * tw,
			(localId / cols) * th,
			tw,
			th
		};
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
}

void WorldParser::addTilesonComponents(ArchetypeManager &manager, EntityID id, tson::Object obj)
{
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

void WorldParser::addPartOfLayerComponents(ArchetypeManager &manager, EntityID id, tson::Object obj,LEVEL_NAME level)
{
	manager.addComponentToEntity<PartOfLayerComponent>(id);
	manager.getComponent<PartOfLayerComponent>(id).level = level;
}

void WorldParser::addBoundingBoxComponents(ArchetypeManager &manager, EntityID id, tson::Object obj)
{
	manager.addComponentToEntity<BoundIngBoxComponent>(id);
}

void WorldParser::createTileObject(std::tuple<tson::TileObject, LEVEL_NAME> & tuple)
{
	auto obj = std::get<0>(tuple);
	auto level = std::get<1>(tuple);
	EntityID id = manager.createEntity();
	manager.addComponentToEntity<TransformComponent>(id);
	manager.getComponent<TransformComponent>(id).position.x = (float)obj.getPosition().x;
	manager.getComponent<TransformComponent>(id).position.y = (float)obj.getPosition().y;
	manager.addComponentToEntity<RenderComponent>(id);
	manager.getComponent<RenderComponent>(id).z_layer = 0;
	manager.addComponentToEntity<SpriteComponent>(id);
	auto &sprite = manager.getComponent<SpriteComponent>(id);

	sprite.tileInfo = {obj.getDrawingRect().x,obj.getDrawingRect().y,obj.getDrawingRect().width,obj.getDrawingRect().height};
	sprite.tilesetPath = fs::path(MAP).parent_path() / obj.getTile()->getTileset()->getImagePath();

	manager.addComponentToEntity<PartOfLayerComponent>(id);
}


void WorldParser::createEntity(std::tuple<tson::Object, LEVEL_NAME> & tuple)
{


	auto obj = std::get<0>(tuple);
	if (obj.getObjectType()!=tson::ObjectType::Rectangle && obj.getObjectType()!=tson::ObjectType::Object) {
		throw std::runtime_error("We allow only rectangle objects");
	}
	auto level = std::get<1>(tuple);
	EntityID id = manager.createEntityWithId(obj.getId());
	addTransformcomponent(manager, id, obj, level);
	addRenderComponent(manager, id, obj);
	addSpriteComponent(manager,id,obj);
	addBoundingBoxComponents(manager, id, obj);
	addPartOfLayerComponents(manager, id, obj, level);
	addTilesonComponents(manager, id, obj);


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
	worldComp.currentLevel = LEVEL_NAME::LEVEL1;
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
		LEVEL_NAME level_name = WorldUtils::getEnumValue<LEVEL_NAME>(configObject,"levelName");
		LAYERTYPE layertype = WorldUtils::getEnumValue<LAYERTYPE>(configObject,"layerType");
		for (auto & obj : objlayer.getObjects()) {
			objects[obj.getId()] = {obj,level_name};
		}
	}

	for (auto &tr: objects) {
		createEntity(tr.second);
	}


	for (auto &tileLayer : tileLayers) {
		tson::Property config = getPropertyByPropertyType(tileLayer.getProperties(), "LayerConfig");
		auto configObject = getCustomPropertyAsClass(config);
		LEVEL_NAME level_name = WorldUtils::getEnumValue<LEVEL_NAME>(configObject,"levelName");
		LAYERTYPE layertype = WorldUtils::getEnumValue<LAYERTYPE>(configObject,"layerType");

		for(auto &[pos, tileObject] : tileLayer.getTileObjects()) {
			std::tuple<tson::TileObject, LEVEL_NAME> obj = std::make_tuple(tileObject, level_name);
			createTileObject(obj);
		}
	}

	

	parseRawEquipmentComponent(manager, worldComp);
	equibFists(manager, worldComp);
}