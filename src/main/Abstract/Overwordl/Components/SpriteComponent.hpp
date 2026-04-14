#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SpriteComponent : public Component<SpriteComponent> {
	TileInfo tileInfo;

	void readFromJson(const nlohmann::json &j) override
	{
	}

	void setTileInfo(int x, int y)
	{
		tileInfo = {x,y};
	}

};