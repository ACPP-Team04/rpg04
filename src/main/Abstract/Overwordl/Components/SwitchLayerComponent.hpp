#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SwitchLayerComponent : public Component<SwitchLayerComponent> {
	LEVEL_NAME level;
	LAYERTYPE layer;
	void readFromJson(const nlohmann::json &j) override
	{
		level = j.value("level", LEVEL_NAME());
		layer = j.value("layer", LAYERTYPE());
	}
};