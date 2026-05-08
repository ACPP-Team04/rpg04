#pragma once
struct PlaySoundEvent {
	std::string soundId;
	sf::Vector2f position;
	bool isSpatial = false;
	float volumeModifier = 1.0f;
};