#pragma once
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/PartOfLayerComponent.hpp"
#include "Components/RenderComponent.hpp"
#include "Components/SpriteComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "tiled/Map.hpp"

#include <memory>

class EntityFactory {

	ArchetypeManager &mgr;
public:
	EntityFactory(ArchetypeManager &archetypeManager) : mgr(archetypeManager) {};


	tson::TiledClass getCustomPropertyAsClass(tson::Property prop)
	{
		return prop.getValue<tson::TiledClass>();
	}

	void addTilesonComponents(EntityID id, tson::Object obj)
	{
		for (auto &prop : obj.getProperties().getProperties()) {
			std::string propType = prop.second.getPropertyType();
			std::cout << "Adding: " << propType << std::endl;
			ComponentRegistry &componentRegistry = ComponentRegistry::getInstance();
			auto func = componentRegistry.getCreationFunctions(propType);
			auto propClass = getCustomPropertyAsClass(prop.second);
			if (func) {

				func(this->mgr, id, propClass);
			}
			std::cout << "Added:" << propType << std::endl;
		}
	}

	void readFromJson(const std::unique_ptr<tson::Map>& map)
	{
		for (auto& layer : map->getLayers()) {
			readFromJson(layer, map, -1);
		}
	}
	void createEntity(tson::Object object,ParseContext &context)
	{
		EntityID id = mgr.createEntityWithId(object.getId());
		ComponentRegistry::getInstance().addDefaultComponent<TransformComponent>(mgr,id,object,context);
		if (object.isVisible()) {
			ComponentRegistry::getInstance().addDefaultComponent<RenderComponent>(mgr,id,object,context);
		}
		ComponentRegistry::getInstance().addDefaultComponent<SpriteComponent>(mgr,id,object,context);
		ComponentRegistry::getInstance().addDefaultComponent<PartOfLayerComponent>(mgr,id,object,context);
		ComponentRegistry::getInstance().addDefaultComponent<StateComponent>(mgr,id,object,context);

		addTilesonComponents(id,object);
	}

	void readFromJson(tson::Layer& layer, const std::unique_ptr<tson::Map>& map, int groupId)
	{
		switch (layer.getType()) {
		case tson::LayerType::Group: {
			for (auto& subLayer : layer.getLayers()) {
				readFromJson(subLayer, map, layer.getId());
			}
			break;
		}
		case tson::LayerType::ObjectGroup: {
			ParseContext context = {groupId, map->getTileSize(),map};
			for (auto& object : layer.getObjects()) {
				createEntity(object, context);
			}
			break;
		}
		default:
			break;
		}
	}
};