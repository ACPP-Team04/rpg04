#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct SwitchLayerComponent : public Component<SwitchLayerComponent> {
	int destination;
	void readFromJson(const nlohmann::json &j) override { destination = j.value("destination", 0); }
};