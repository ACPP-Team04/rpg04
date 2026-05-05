#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Audio/AudioManager.hpp"
#include <SFML/Audio.hpp>
#include <spdlog/spdlog.h>

AudioManager::AudioManager(size_t poolSize) : masterMusicVolume(100.0f) {}

void AudioManager::switchMusic(const std::string &musicName, bool loop) {}
void AudioManager::stopMusic() {}
void AudioManager::setMasterMusicVolume(float volume) {}
void AudioManager::playMusic(const std::string &musicName, bool loop)
{
	auto musicPath = AssetManager::getInstance().getMusicPath(musicName);
	if (!backgroundMusic.openFromFile(musicPath)) {
		spdlog::error("Failed to open music file at: {}", musicPath);
		return;
	}
	backgroundMusic.setLooping(loop);
	backgroundMusic.play();

	spdlog::info("Now playing: {}", musicName);
}
