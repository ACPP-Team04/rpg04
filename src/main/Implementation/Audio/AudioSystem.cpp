#include "Abstract/Audio/AudioSystem.hpp"
#include "Abstract/Audio/AudioManager.hpp"
#include "Abstract/Overwordl/Components/AudioComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

AudioSystem::AudioSystem(ArchetypeManager &manager, AudioManager &audioManager)
    : System(manager), audioManager(audioManager)
{
}

void AudioSystem::enqueueSound(std::string_view soundId, float volumeModifier)
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

	manager.view<AudioComponent, TransformComponent>().each(
	    [this]([[maybe_unused]] EntityID id, AudioComponent &acomp, const TransformComponent &tcomp) {
		    auto playerPosOptional = WorldUtils::getPlayersComponent<TransformComponent>(manager);

		    if (!playerPosOptional.has_value()) {
			    return;
		    }
		    auto playerPos = playerPosOptional->get().position;
		    float distSq = SwitchBattleModeSystem::getSquaredDistance(tcomp.position, playerPos);
		    float maxDistSq = acomp.maxDistance * acomp.maxDistance;

		    if (distSq <= maxDistSq) {
			    if (acomp.activeSound == nullptr || acomp.activeSound->getStatus() == sf::Sound::Status::Stopped) {
				    acomp.activeSound = audioManager.playLoopingSound(acomp.soundId);
			    }
			    if (acomp.activeSound != nullptr) {
				    float distance = std::sqrt(distSq);
				    float volumeFactor = 1.0f - (distance / acomp.maxDistance);
				    acomp.activeSound->setVolume(acomp.maxVolume * volumeFactor);
			    }
		    } else {
			    if (acomp.activeSound != nullptr) {
				    acomp.activeSound->stop();
				    acomp.activeSound = nullptr;
			    }
		    }
	    });
}

void AudioSystem::switchMusic(const std::string &musicName, bool loop)
{
	audioManager.switchMusic(musicName, loop);
}

void AudioSystem::stopMusic()
{
	audioManager.stopMusic();
}