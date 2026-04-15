#pragma once
#include "Abstract/ECS/Component/Component.hpp"

#include <nlohmann/json_fwd.hpp>
struct ITEM_HEALSTATS_COMPONENT : public Component<ITEM_HEALSTATS_COMPONENT> {
	int healAmount;
	void readFromJson(tson::TiledClass &j) override

	{
		healAmount = j.get<int>("healAmount");
	}
};
