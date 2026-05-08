#include "Abstract/Audio/AudioSystem.hpp"
#include "Abstract/Audio/AudioManager.hpp"

AudioSystem::AudioSystem(ArchetypeManager &manager, AudioManager &audioManager)
    : System(manager), audioManager(audioManager)
{
}

void AudioSystem::enqueueSound(const std::string &soundId, float volumeModifier)
{
	PlaySoundEvent event;
	event.soundId = soundId;
	event.volumeModifier = volumeModifier;
	soundQueue.push_back(event);
}

void AudioSystem::update()
{
	for (const auto &event : soundQueue) {
		audioManager.playSound(event.soundId, event.volumeModifier);
	}

	soundQueue.clear();

	// Add later the code that searches for all AudioEmitterComponents (campfire), calculates the distance and adjusts
	// the volume.
}