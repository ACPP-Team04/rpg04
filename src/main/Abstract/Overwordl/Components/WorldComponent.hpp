#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <unordered_map>



struct WorldComponent : public Component<WorldComponent> {
	unsigned widthPixel;
	unsigned heightPixel;

	LEVEL_NAME currentLevel = (LEVEL_NAME)0;
	LAYERTYPE currentLayer = (LAYERTYPE)0;
	std::unordered_map<LAYERTYPE, MENUS> menus;
	bool menuOpened = false;

	void readFromJson(tson::TiledClass &j)
	{
	}

};