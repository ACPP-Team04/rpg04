#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "DefaultComponent.hpp"

struct PartOfLayerComponent : public Component<PartOfLayerComponent>,DefaultComponent {
	int groupId;
	void readFromJson(tson::TiledClass &j) override {}
	void readFromObject(tson::Object &object, ParseContext &context) override
	{
		groupId = context.groupId;
	}
};
