#pragma once
#include <SFML/Audio.hpp>
#include <list>
#include <string>

class AudioManager {
  private:
	sf::Music backgroundMusic;
	std::list<sf::Sound> soundPool;
	size_t maxPoolSize;
	float masterMusicVolume;
	float masterSfxVolume;

  public:
	AudioManager(size_t poolSize = 16);
	void switchMusic(const std::string &musicName, bool loop = true);
	void playMusic(const std::string &musicName, bool loop = true);
	void stopMusic();
	void setMasterMusicVolume(float volume);
	void setMasterSfxVolume(float volume);
	void playSound(const std::string &soundName, float volumeModifier);
};
