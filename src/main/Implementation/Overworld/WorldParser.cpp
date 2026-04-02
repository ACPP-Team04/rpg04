#include "Abstract/Overwordl/WorldParser.hpp"

#include "Abstract/Overwordl/Components.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>
#include <utility>

using json = nlohmann::json;
WorldParser::WorldParser(ArchetypeManager &manager, sf::RenderWindow& window):System(manager),window(window)
{

}


void addComponent(EntityID& entity_id,int xLayerPostion,int yLayerPosition ,tileProperty tile_property,ArchetypeManager& manager)
{
	if (tile_property.propertytype == "TRANSFORM_COMPONENT") {
		manager.addComponentToEntity<TransformComponent>(entity_id);
		if (tile_property.value !=0) {
			manager.getComponent<TransformComponent>(entity_id).readFromJson(tile_property.value);
		}
		manager.getComponent<TransformComponent>(entity_id).position.x = (unsigned)xLayerPostion;
		manager.getComponent<TransformComponent>(entity_id).position.y = (unsigned)yLayerPosition;
	}

	if (tile_property.propertytype == "RENDER_COMPONENT") {
		manager.addComponentToEntity<RenderComponent>(entity_id);
		manager.getComponent<RenderComponent>(entity_id).readFromJson(tile_property.value);
	}

	if (tile_property.propertytype == "INPUT_COMPONENT") {
		manager.addComponentToEntity<InputComponent>(entity_id);
		manager.getComponent<InputComponent>(entity_id).readFromJson(tile_property.value);
	};

	if (tile_property.propertytype =="MOVEMENT_COMPONENT") {
		manager.addComponentToEntity<MovementComponent>(entity_id);
		manager.getComponent<MovementComponent>(entity_id).readFromJson(tile_property.value);
	}

	if (tile_property.propertytype =="CAMERA_COMPONENT") {
		manager.addComponentToEntity<CameraComponent>(entity_id);
		manager.getComponent<CameraComponent>(entity_id).readFromJson(tile_property.value);
	}

}

void createEntities(ObjectLayerObject obj,ArchetypeManager &manager)
{
	EntityID entity = manager.createEntity();
	for (tileProperty prop : obj.properties) {
		addComponent(entity,obj.x, obj.y, prop, manager);
	}

}

void WorldParser::update()
{
	std::ifstream f(MAP);
	json data = json::parse(f);
	EntityID world = manager.createEntity<WorldComponent>();
	auto& component = manager.getComponent<WorldComponent>(world);
	component.readFromJson(data);

	window.setSize({component.widthPixel,component.heightPixel});

	sf::View view(sf::FloatRect(
	{0.f, 0.f},
	{static_cast<float>(component.widthPixel),static_cast<float>(component.heightPixel)}
	));
	window.setView(view);

	for (const auto& layer : component.tileLayers) {
		for (int y = 0; y < component.height; ++y) {
			for (int x = 0; x < component.width; ++x) {

				int flatIndex = x + (y * component.width);

				EntityID entity = manager.createEntity();

				for (const tileProperty& prop : layer.tileIds[flatIndex].properties) {
					addComponent(entity,x*component.tilewidth,y*component.tileheight,prop,manager);
				}
			}
		}
	}
	for (const auto& layer : component.objectLayers) {
		for (const auto& obj : layer.objects) {
			createEntities(obj,manager);
		}
	}

}