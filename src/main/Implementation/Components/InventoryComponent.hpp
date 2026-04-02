#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

struct InventoryComponent : Component<InventoryComponent> {
  public:
	int numberOfHealthPotions{1};
	void readFromJson(const nlohmann::json &j) override {}
};
