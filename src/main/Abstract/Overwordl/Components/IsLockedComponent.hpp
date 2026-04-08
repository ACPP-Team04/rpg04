#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct IsLockedComponent : public Component<IsLockedComponent> {
	bool isLocked;
	int keyId;

	void readFromJson(const nlohmann::json &j) override
	{
		isLocked = j.value("isLocked", true);
		keyId = j.value("keyId", 0);
	}
};