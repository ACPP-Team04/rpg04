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

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "tileson.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <fstream>
#include <utility>

WorldParser::WorldParser(ArchetypeManager &manager, sf::RenderWindow &window)
    : System(manager), window(window), project(PRO),t(&project)
{
	map = t.parse(fs::path(MAP));
	std::cout << "Anzahl geladener Tilesets: " << map->getTilesets().size() << std::endl;
	for(auto &tileset : map->getTilesets()) {
		std::cout << "Tileset Name: " << tileset.getName() << " | Tiles Anzahl: " << tileset.getTiles().size() << std::endl;
	}
	std::ifstream f(MAP);
	rawJson = nlohmann::json::parse(f);
}
/*
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
			std::cout << manager.getComponent<StatsComponent>(entity).health << std::endl;
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

*/


tson::TiledClass jsonToTiledClass(const nlohmann::json &json)
{
	tson::TiledClass cls;
	for (auto &[key, value] : json.items()) {
		if (value.is_array()) {
			tson::TiledClass arrayClass;
			for (int i = 0; i < (int)value.size(); i++) {
				nlohmann::json element = value[i];
				if (element.is_object() && element.contains("type") && element["type"] == "class" && element.contains("value")) {
					element = element["value"];
				}
				arrayClass.getMembers().add(std::to_string(i), jsonToTiledClass(element), tson::Type::Class);
				int id = 1;
			}
			cls.getMembers().add(key, arrayClass, tson::Type::Class);
		} else if (value.is_object()) {
			cls.getMembers().add(key, jsonToTiledClass(value), tson::Type::Class);
		} else if (value.is_string()) {
			cls.getMembers().add(key, value.get<std::string>(), tson::Type::String);
		} else if (value.is_number_integer()) {
			cls.getMembers().add(key, value.get<int>(), tson::Type::Int);
		} else if (value.is_number_float()) {
			cls.getMembers().add(key, value.get<float>(), tson::Type::Float);
		} else if (value.is_boolean()) {
			cls.getMembers().add(key, value.get<bool>(), tson::Type::Boolean);
		}
	}
	return cls;
}
void patchLayerRecursive(nlohmann::json &layer, tson::Layer *tsonLayer)
{
	if (!tsonLayer) return;

	if (layer.contains("objects")) {
		for (auto &rawObj : layer["objects"]) {
			if (!rawObj.contains("properties")) continue;
			tson::Object *obj = tsonLayer->getObj(rawObj["id"].get<int>());
			if (!obj) continue;
			for (auto &prop : rawObj["properties"]) {
				if (prop["type"] != "class") continue;
				bool hasArray = false;
				for (auto &[k, v] : prop["value"].items()) {
					if (v.is_array()) { hasArray = true; break; }
				}
				if (!hasArray) continue;
				std::string propType = prop["propertytype"].get<std::string>();
				std::string propName = prop["name"].get<std::string>(); // "animation"
				tson::TiledClass cls = jsonToTiledClass(prop["value"]);
				obj->getProperties().getProperty(propName)->setValue(cls);
				int g = 0;
			}
		}
	}
	if (layer.contains("layers")) {
		auto& subLayers = tsonLayer->getLayers();
		for (int i = 0; i < (int)layer["layers"].size(); i++) {
			patchLayerRecursive(layer["layers"][i], &subLayers[i]);
		}
	}

}

void patchArrayProperties(nlohmann::json &rawMapJson, const std::unique_ptr<tson::Map>& map)
{
	auto& tsonLayers = map->getLayers();
	for (int i = 0; i < (int)rawMapJson["layers"].size(); i++) {
		patchLayerRecursive(rawMapJson["layers"][i], &tsonLayers[i]);
	}
}

void WorldParser::update()
{


	patchArrayProperties(rawJson, map);
	EntityID world = manager.createEntity<WorldComponent>();

	WorldComponent &worldComp = manager.getComponent<WorldComponent>(world);
	worldComp.readFromJson(map,rawJson);

	EntityFactory factory = EntityFactory(manager);
	factory.readFromJson(map);
	EntityID player = WorldUtils::getPlayer(manager).value();
	worldComp.currentGroup = manager.getComponent<PartOfLayerComponent>(player).groupId;

	/*
	parseDialogComponent(manager, rawJson, objectsWithDialogComponent);
	parseRawEquipmentComponent(manager, worldComp);
	equibFists(manager, worldComp);*/
}