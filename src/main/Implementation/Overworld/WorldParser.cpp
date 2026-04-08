#include "Abstract/Overwordl/WorldParser.hpp"

#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/Overwordl/Components/BoundingBoxComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

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
	std::cout << "Adding: " << tile_property.propertytype << std::endl;
	ComponentRegistry &componentRegistry = ComponentRegistry::getInstance();
	auto func = componentRegistry.getCreationFunctions(tile_property.propertytype);
	if (func) {
		func(manager, entity_id, tile_property.value);
	}
}

void addBoundingBox(ArchetypeManager &manager, EntityID &entity_id)
{
	manager.addComponentToEntity<BoundIngBoxComponent>(entity_id);
}

void addPartLayerComponent(ArchetypeManager &manager, EntityID &entity_id, LEVEL_NAME level, LAYERTYPE layer)
{

	manager.addComponentToEntity<PartOfLayerComponent>(entity_id);
	manager.getComponent<PartOfLayerComponent>(entity_id).layer = layer;
	manager.getComponent<PartOfLayerComponent>(entity_id).level = level;
}

void createEntities(const ObjectLayerObject &obj, ArchetypeManager &manager, LEVEL_NAME level, LAYERTYPE layer)
{
	EntityID entity = manager.createEntityWithId(obj.id);
	addPartLayerComponent(manager, entity, level, layer);
	addBoundingBox(manager, entity);
	for (const tileProperty &prop : obj.properties) {
		addComponent(entity, obj.x, obj.y, prop, manager);
	}
	if (manager.hasComponent<RenderComponent>(entity)) {
		manager.getComponent<RenderComponent>(entity).z_layer = 1;
	}
}

void intializeEntities(const WorldLayer &worldLayer, ArchetypeManager &manager, const WorldComponent &component,
                       LEVEL_NAME level, LAYERTYPE layer)
{
	for (const auto &objLayer : worldLayer.objectLayers) {
		for (const auto &obj : objLayer.objects) {
			createEntities(obj, manager, level, layer);
		}
	}
	for (const auto &tileLayer : worldLayer.tileLayers) {
		for (int y = 0; y < component.height; ++y) {
			for (int x = 0; x < component.width; ++x) {
				int flatIndex = x + (y * component.width);
				EntityID entity = manager.createEntity();
				addPartLayerComponent(manager, entity, level, layer);
				addBoundingBox(manager, entity);
				for (const tileProperty &prop : tileLayer.tileIds[flatIndex].properties) {
					addComponent(entity, x * component.tilewidth, y * component.tileheight, prop, manager);
				}

				if (manager.hasComponent<RenderComponent>(entity)) {
					manager.getComponent<RenderComponent>(entity).z_layer = 0;
				}
			}
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

	for (const auto &[levelName, levelLayer] : component.levelLayers) {
		for (const auto &[layerType, layer] : levelLayer.layers) {
			intializeEntities(layer, manager, component, levelName, layerType);
		}
	}

	component.register_menu(OVERWORLD, MENUS::Menu1);
	component.register_menu(BATTLEWORLD, MENUS::Menu2);
}
