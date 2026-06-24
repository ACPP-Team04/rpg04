#pragma once
#include "Abstract/Audio/AudioManager.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "PlaySoundEvent.hpp"
#include <string>
#include <vector>

class AudioSystem : public System {
  private:
	AudioManager &audioManager;
	std::vector<PlaySoundEvent> soundQueue;

  public:
	AudioSystem(ArchetypeManager &manager, AudioManager &audioManager);
	void update() override;
	void enqueueSound(std::string_view, float volumeModifier = 1.0f);
	void switchMusic(const std::string &musicName, bool loop);
	void stopMusic();
};