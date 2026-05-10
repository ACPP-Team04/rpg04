#include "Abstract/Audio/AudioManager.hpp"
#include "Abstract/AssetManager/AssetManager.hpp"
#include <SFML/Audio.hpp>
#include <spdlog/spdlog.h>

AudioManager::AudioManager(size_t poolSize) : masterMusicVolume(25.0f), masterSfxVolume(100.0f), maxPoolSize(poolSize)
{
	spdlog::info("AudioManager initialized with a max of {} sound channels.", poolSize);
}

void AudioManager::switchMusic(const std::string &musicName, bool loop)
{
	stopMusic();
	playMusic(musicName, loop);
}
void AudioManager::stopMusic()
{
	backgroundMusic.stop();
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
	auto musicPath = AssetManager::getInstance().getMusicPath(musicName);
	if (!backgroundMusic.openFromFile(musicPath)) {
		spdlog::error("Failed to open music file at: {}", musicPath);
		return;
	}
	backgroundMusic.setLooping(loop);
	backgroundMusic.setVolume(masterMusicVolume);
	backgroundMusic.play();

	spdlog::info("Now playing: {}", musicName);
}
void AudioManager::playSound(const std::string &soundName, float volumeModifier)
{
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
		spdlog::error("AudioManager: Cannot play sound because buffer is missing.");
	}
}
sf::Sound *AudioManager::playLoopingSound(const std::string &soundId, float startVolume)
{
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