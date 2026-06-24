#include "Abstract/Audio/AudioManager.hpp"
#include "Abstract/AssetManager/AssetManager.hpp"
#include <SFML/Audio.hpp>
#include <spdlog/spdlog.h>

AudioManager::AudioManager(size_t poolSize, bool headless) : maxPoolSize(poolSize), isHeadless(headless)
{
	if (!headless) {
		spdlog::info("AudioManager initialized with a max of {} sound channels.", poolSize);
	} else {
		spdlog::info("AudioManager initialized headless.");
	}
}

void AudioManager::switchMusic(const std::string &musicName, bool loop)
{
	stopMusic();
	playMusic(musicName, loop);
}
void AudioManager::stopMusic()
{
	backgroundMusic.stop();
	currentMusicName = std::nullopt;
}
void AudioManager::setMasterMusicVolume(float volume)
{
	masterMusicVolume = volume;
	backgroundMusic.setVolume(volume);
}

void AudioManager::setMasterSfxVolume(float volume)
{
	masterSfxVolume = volume;
	for (auto &sound : soundPool) {
		sound.setVolume(volume);
	}
}
void AudioManager::playMusic(const std::string &musicName, bool loop)
{
	if (isHeadless)
		return;

	if (auto musicPath = AssetManager::getInstance().getMusicPath(musicName);
	    !backgroundMusic.openFromFile(musicPath)) {
		spdlog::error("Failed to open music file at: {}", musicPath);
		return;
	}
	backgroundMusic.setLooping(loop);
	backgroundMusic.setVolume(masterMusicVolume);
	backgroundMusic.play();
	currentMusicName = musicName;
	spdlog::info("Now playing: {}", musicName);
}
void AudioManager::playSound(const std::string &soundName, float volumeModifier)
{
	if (isHeadless)
		return;
	try {
		sf::SoundBuffer &buffer = AssetManager::getInstance().getSoundBuffer(soundName);
		soundPool.remove_if([](const sf::Sound &s) { return s.getStatus() == sf::Sound::Status::Stopped; });

		if (soundPool.size() >= maxPoolSize) {
			spdlog::warn("All sound channels are busy! Could not play '{}'", soundName);
			return;
		}
		soundPool.emplace_back(buffer);
		soundPool.back().setVolume(masterSfxVolume * volumeModifier);
		soundPool.back().play();

	} catch (const std::runtime_error &e) {
		spdlog::error("AudioManager: Cannot play sound because buffer is missing: {}", e.what());
	}
}
sf::Sound *AudioManager::playLoopingSound(const std::string &soundId, float startVolume)
{
	if (isHeadless)
		return nullptr;
	if (soundPool.size() >= maxPoolSize) {
		spdlog::warn("AudioManager Pool is full. Ignoring {}", soundId);
		return nullptr;
	}
	try {
		sf::SoundBuffer &buffer = AssetManager::getInstance().getSoundBuffer(soundId);
		soundPool.emplace_back(buffer);
		sf::Sound &newSound = soundPool.back();
		newSound.setLooping(true);

		float finalVolume = masterSfxVolume * (startVolume / 100.0f);
		newSound.setVolume(finalVolume);
		newSound.play();

		return &newSound;

	} catch (const std::exception &e) {
		spdlog::error("Error playing {}: {}", soundId, e.what());
		return nullptr;
	}
}
void AudioManager::pauseAll()
{
	if (isHeadless)
		return;
	if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
		backgroundMusic.pause();
	}

	for (auto &sound : soundPool) {
		if (sound.getStatus() == sf::Sound::Status::Playing) {
			sound.pause();
		}
	}

	spdlog::info("Audio globally paused.");
}

void AudioManager::resumeAll()
{
	if (isHeadless)
		return;
	if (backgroundMusic.getStatus() == sf::Music::Status::Paused) {
		backgroundMusic.play();
	}
	for (auto &sound : soundPool) {
		if (sound.getStatus() == sf::Sound::Status::Paused) {
			sound.play();
		}
	}

	spdlog::info("Audio globally resumed.");
}