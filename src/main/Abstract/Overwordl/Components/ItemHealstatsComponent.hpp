#pragma once
#include "Abstract/ECS/Component/Component.hpp"

#include <nlohmann/json_fwd.hpp>
struct ITEM_HEALSTATS_COMPONENT : public Component<ITEM_HEALSTATS_COMPONENT> {
	int healAmount;
	void readFromJson(const nlohmann::json &j) override { healAmount = j.value("healAmount", 1); }
};
