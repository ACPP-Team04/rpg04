#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/GlobalProperties.hpp"
#include "EntityFactory.hpp"

#include "Abstract/TILE_ENUMS.hpp"
#include "tileson.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <nlohmann/json.hpp>
#include <algorithm>
struct WorldParser : System {
	tson::Project project;
	sf::RenderWindow &window;
	tson::Tileson t;
	std::unique_ptr<tson::Map> map;
	nlohmann::json rawJson;

	WorldParser(ArchetypeManager &manager, sf::RenderWindow &window);

	void parseListProperties();

	void addAnimationComponent(ArchetypeManager &manager, EntityID id, nlohmann::json &data);
	void parseDialogComponent(ArchetypeManager &manager, nlohmann::json &data,
	                          std::unordered_map<int, nlohmann::json> &objectsWithComponent);
	void update() override;
};