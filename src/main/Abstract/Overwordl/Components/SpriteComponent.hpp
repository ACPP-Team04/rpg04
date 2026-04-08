#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SpriteComponent : public Component<SpriteComponent> {
	TileType textureId;

	void readFromJson(const nlohmann::json &j) override
	{
		this->textureId = static_cast<TileType>(j.value("texture", 0));
	}
};