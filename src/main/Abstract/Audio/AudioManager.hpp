#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class AudioManager {
  private:
	sf::Music backgroundMusic;
	float masterMusicVolume;

  public:
	AudioManager(size_t poolSize = 16);
	void switchMusic(const std::string &musicName, bool loop = true);
	void playMusic(const std::string &musicName, bool loop = true);
	void stopMusic();
	void setMasterMusicVolume(float volume);
};
