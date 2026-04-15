#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SwitchLayerComponent : public Component<SwitchLayerComponent> {
	int destination;
	void readFromJson(tson::TiledClass &j) override
	{
		destination = j.get<int>("destination");
	}
};