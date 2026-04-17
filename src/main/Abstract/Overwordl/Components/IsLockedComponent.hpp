#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct IsLockedComponent : public Component<IsLockedComponent> {
	bool isLocked;
	int keyId;

	void readFromJson(tson::TiledClass &j) override
	{
		isLocked = j.get<bool>("isLocked");
		keyId = j.get<int>("keyId");
	}
};