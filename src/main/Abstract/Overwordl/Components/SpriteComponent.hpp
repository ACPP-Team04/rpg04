#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SpriteComponent : public Component<SpriteComponent> {
	TileInfo tileInfo;
	std::string tilesetPath;

	void readFromJson(tson::TiledClass &j) override

	{
	}

};