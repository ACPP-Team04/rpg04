#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

struct InventoryComponent : public Component<InventoryComponent> {
	std::vector<EntityID> inventory;

	void readFromJson(const nlohmann::json &j) override {}
};