#pragma once
#include "Abstract/ECS/Component/Component.hpp"
struct AudioComponent : Component<AudioComponent> {
	std::string soundId;
	float maxDistance;
	float maxVolume = 100.0f;
	sf::Sound *activeSound = nullptr;

	void readFromJson(tson::TiledClass &j) override
	{

		soundId = j.get<std::string>("sound_id");
		maxDistance = j.get<float>("max_distance");
	}
};