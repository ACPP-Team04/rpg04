#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/GlobalProperties.hpp"

#include "Abstract/TILE_ENUMS.hpp"
#include "tileson.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <nlohmann/json.hpp>

struct WorldParser : System {
	tson::Project project;
	sf::RenderWindow &window;
	std::unique_ptr<tson::Map> map;
	WorldParser(ArchetypeManager &manager, sf::RenderWindow &window);
	void addBoundingBoxComponents(ArchetypeManager &manager, EntityID id, tson::Object obj);
	void createTileObject(std::tuple<tson::TileObject, LEVEL_NAME> &tuple);
	void createEntity(std::tuple<tson::Object, LEVEL_NAME> & tuple);
	void update() override;
	void undfoldLayers(std::vector<tson::Layer>& layer,std::vector<tson::Layer> &objectLayers, std::vector<tson::Layer> &tileLayers);
	void addTransformcomponent(ArchetypeManager &manager, EntityID id, tson::Object obj,LEVEL_NAME level);
	void addRenderComponent(ArchetypeManager &manager, EntityID id, tson::Object obj);
	void addSpriteComponent(ArchetypeManager &manager, EntityID id, tson::Object obj);
	void addTilesonComponents(ArchetypeManager &manager, EntityID id, tson::Object obj);
	void addPartOfLayerComponents(ArchetypeManager &manager, EntityID id, tson::Object obj, LEVEL_NAME level);
};