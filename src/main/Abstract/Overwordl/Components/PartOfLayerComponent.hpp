#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct PartOfLayerComponent : public Component<PartOfLayerComponent> {
	LEVEL_NAME level;
	LAYERTYPE layer;
	void readFromJson(tson::TiledClass &j) override {}
};
