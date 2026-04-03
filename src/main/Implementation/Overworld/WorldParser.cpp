#include "Abstract/Overwordl/WorldParser.hpp"

#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/Overwordl/Components.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>
#include <utility>

using json = nlohmann::json;
WorldParser::WorldParser(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {}

void addComponent(EntityID &entity_id, int xLayerPostion, int yLayerPosition, tileProperty tile_property,
				  ArchetypeManager &manager)
{
	tile_property.value["position_x"] = xLayerPostion;
	tile_property.value["position_y"] = yLayerPosition;

	ComponentRegistry& componentRegistry = ComponentRegistry::getInstance();
	auto func = componentRegistry.getCreationFunctions(tile_property.propertytype);
	if (func) {
		func(manager, entity_id, tile_property.value);
	}
	std::cout << tile_property.propertytype << std::endl;
}

void addPartLayerComponent(ArchetypeManager &manager,EntityID &entity_id, int level, std::string layer)
{
	manager.addComponentToEntity<PartOfLayerComponent>(entity_id);
	manager.getComponent<PartOfLayerComponent>(entity_id).layer = layer;
	manager.getComponent<PartOfLayerComponent>(entity_id).level = level;
}

void createEntities(const ObjectLayerObject& obj, ArchetypeManager &manager, int level, const std::string& layer)
{
	EntityID entity = manager.createEntity();
	addPartLayerComponent(manager, entity, level, layer);
	for (const tileProperty& prop : obj.properties) {
		addComponent(entity, obj.x, obj.y, prop, manager);
	}
}

void intializeEntities(const WorldLayer &worldLayer, ArchetypeManager &manager, const WorldComponent &component, int level, const std::string& layer)
{
	for (const auto &tileLayer : worldLayer.tileLayers) {
		for (int y = 0; y < component.height; ++y) {
			for (int x = 0; x < component.width; ++x) {
				int flatIndex = x + (y * component.width);
				EntityID entity = manager.createEntity();
				addPartLayerComponent(manager, entity, level, layer);
				for (const tileProperty &prop : tileLayer.tileIds[flatIndex].properties) {
					addComponent(entity, x * component.tilewidth, y * component.tileheight, prop, manager);
				}
			}
		}
	}
	for (const auto &objLayer : worldLayer.objectLayers) {
		for (const auto &obj : objLayer.objects) {
			createEntities(obj, manager, level, layer);
		}
	}
}

void WorldParser::update()
{
	std::ifstream f(MAP);
	json data = json::parse(f);
	EntityID world = manager.createEntity<WorldComponent>();
	auto &component = manager.getComponent<WorldComponent>(world);
	component.readFromJson(data);

	window.setSize({component.widthPixel, component.heightPixel});

	sf::View view(sf::FloatRect({0.f, 0.f},
	                            {static_cast<float>(component.widthPixel), static_cast<float>(component.heightPixel)}));
	window.setView(view);

	int level = 0;
	for (const auto &layer : component.levelLayers) {
		for (WorldLayer obj : layer.overworld_layer) {
			intializeEntities(obj, manager, component,level,OVERWORLD_LAYER);
		}
		for (WorldLayer obj : layer.battle_layer) {
			intializeEntities(obj, manager, component,level,BATTLE_LAYER);
		}
	}


}

