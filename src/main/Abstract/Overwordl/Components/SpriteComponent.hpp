#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
struct TileInfo {
	int pixelX;
	int pixelY;
	int width;
	int height;
};

struct SpriteComponent : public Component<SpriteComponent> {
	TileInfo tileInfo;
	std::string tilesetPath;

	void readFromJson(tson::TiledClass &j) override

	{
	}

};