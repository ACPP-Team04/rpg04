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
	bool isHeadless;
	AudioManager(size_t poolSize = 16, bool headless = false);
  public:
	void switchMusic(const std::string &musicName, bool loop = true);
	void playMusic(const std::string &musicName, bool loop = true);
	void stopMusic();
	void setMasterMusicVolume(float volume);
	void setMasterSfxVolume(float volume);
	void playSound(const std::string &soundName, float volumeModifier);
	sf::Sound *playLoopingSound(const std::string &soundId, float startVolume = 100.0f);
	void pauseAll();
	void resumeAll();
	static AudioManager &getInstance()
	{
	  static AudioManager instance = AudioManager();
		return instance;
	}

};
