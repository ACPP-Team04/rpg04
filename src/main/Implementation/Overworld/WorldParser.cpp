#include "Abstract/Overwordl/WorldParser.hpp"

#include "Abstract/Overwordl/Components.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>

using json = nlohmann::json;
WorldParser::WorldParser(ArchetypeManager &manager, sf::RenderWindow& window):System(manager),window(window)
{

}
template <typename T>
EntityID initalizeComponent(ArchetypeManager &manager, json &data)
{
	EntityID world = manager.createEntity<T>();
	manager.getComponent<WorldComponent>(world).readFromJson(data);
	return world;
}

template <typename T>
EntityID initalizeComponent(ArchetypeManager &manager, T& comp)
{
	EntityID world = manager.createEntity<T>();
	manager.getComponent<T>(world) = std::move(comp);
	return world;
}
template <typename T>
void addComponent(EntityID entity_id, ArchetypeManager &manager, T& comp)
{

	manager.addComponentToEntity<T>(entity_id);
	manager.getComponent<T>(entity_id) = std::move(comp);
}
template <typename T>
void addComponent(EntityID entity_id, ArchetypeManager &manager, json& data)
{

	manager.addComponentToEntity<T>(entity_id);
	manager.getComponent<T>(entity_id).readFromJson(data);
}



struct tileProperty {
	std::string name;
	std::string type;
	nlohmann::json value;
	bool isPartOfIbject;
	int id;
	int width;
	int height;



	void readFromJson(const json &data)
	{
		name = data.value("name","");
		type = data.value("type","");
		value = data.value("value",nlohmann::json());
		std::string part = data.value("part","") ;
		isPartOfIbject = (!part.empty()) && (part != "00");
		id = data.value("id",0);


	}
};


static int OFFSET = 1;
std::vector<tileProperty>JsonToEntity(WorldComponent& world,int searchTileId, json &data, ArchetypeManager &manager)
{

	std::vector<tileProperty>  properties;

	for (auto tileset: data.value("tilesets",json::array())) {
		for (auto tile: tileset.value("tiles", json::array())) {
			int tile_id = tile.value("id", 0);
			if (searchTileId != tile_id) {
				continue;
			}
			std::string part = "";
			for (auto prop : tile.value("properties", json::array())) {
				if (prop.value("name","") == "part") {
					part = prop.value("value","");
					break;
				}
			}
			bool isSubTile = (!part.empty()) && (part != "00");
			if (isSubTile) {
				break;
			}

			for (auto prop : tile.value("properties", json::array())) {
				std::string propName = prop.value("name","");
				std::string propType = prop.value("type","");
				if (propType != "object") continue;

				tileProperty property;
				property.name = propName;
				property.type = propType;

				TileType tileType = TileType(tile_id);
				if (TILE_DICT.find(tileType) != TILE_DICT.end()) {
					property.height = world.tileheight * TILE_DICT.at(tileType).size();
					if (property.height > 0)
						property.width = world.tilewidth * TILE_DICT.at(tileType)[0].size();
				}
				properties.push_back(property);
			}
			break;
		}
	}
	return properties;

}
void WorldParser::update()
{
	std::ifstream f(MAP);
	json data = json::parse(f);
	for (auto layer :data.value("layers",json::array())) {
		EntityID world = initalizeComponent<WorldComponent>(this->manager, data);
		auto& currentWorld = manager.getComponent<WorldComponent>(world);
		currentWorld.width = data.value("width",0);
		currentWorld.height = data.value("height",0);
		this->window.setSize({(unsigned)currentWorld.width*currentWorld.tilewidth,(unsigned)currentWorld.height*currentWorld.tileheight});

		int flatIndex =0;
		int i = 0;
		int j = 0;
		for (auto tile : layer.value("data",json::array())) {
			int searchId = (int)tile-OFFSET;
			std::vector<tileProperty> properties = JsonToEntity(currentWorld,searchId,data,this->manager);
			i = flatIndex / currentWorld.width;
			j = flatIndex % currentWorld.width;
			if (properties.size() == 0) {
				flatIndex++;
				continue;
			}
			EntityID entity = manager.createEntity();
			for (auto prop: properties) {
				if (prop.name == "TRANSFORM_COMPONENT" || prop.name == "TRANSFROM_COMPONENT") {
					TransformComponent comp;
					comp.position.x = (float)j * prop.width;
					comp.position.y = (float)i * prop.height;
					addComponent<TransformComponent>(entity,this->manager,comp);
				}

				if (prop.name == "INPUT_COMPONENT") {
					addComponent<InputComponent>(entity,this->manager,data);
				}

				if (prop.name == "RENDER_COMPONENT") {
					RenderComponent comp;
					comp.activeTiles = static_cast<TileType>(searchId);
					addComponent<RenderComponent>(entity,this->manager,comp);
				}
			}
			flatIndex++;
		}
	}
}