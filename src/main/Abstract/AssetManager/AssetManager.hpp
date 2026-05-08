#pragma once
#include "Abstract/GlobalProperties.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <spdlog/spdlog.h>
#include <vector>
struct AssetManager {

	~AssetManager() = default;
	AssetManager(const AssetManager &) = delete;
	AssetManager &operator=(const AssetManager &) = delete;
	AssetManager(AssetManager &&) = delete;
	AssetManager &operator=(AssetManager &&) = delete;
	static AssetManager &getInstance()
	{
		static AssetManager instance = AssetManager();
		return instance;
	}

	std::unordered_map<std::string, std::shared_ptr<sf::Texture>> chacheImage = {};

	sf::Sprite getSpriteAt(SpriteComponent &tile)
	{
		if (!chacheImage.contains(tile.tilesetPath)) {
			if (!textureSet->loadFromFile(tile.tilesetPath)) {
				throw std::runtime_error("Failed to load texture");
			}
			chacheImage[tile.tilesetPath] = textureSet;
		}
		return {*chacheImage[tile.tilesetPath],
		        sf::IntRect({tile.tileInfo.pixelX, tile.tileInfo.pixelY}, {tile.tileInfo.width, tile.tileInfo.height})};
	}

	std::unordered_map<std::string, sf::SoundBuffer> soundBuffers = {};
	std::unordered_map<std::string, std::string> musicPaths = {};

	void registerMusic(const std::string &name, const std::string &filepath)
	{
		musicPaths[name] = filepath;
		spdlog::info("Registered Music Track: {}", name);
	}

	std::string getMusicPath(const std::string &name)
	{
		if (!musicPaths.contains(name)) {
			spdlog::warn("Music path '{}' not found!", name);
			return "";
		}
		return musicPaths[name];
	}
	void registerSound(const std::string &name, const std::string &filepath)
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(filepath)) {
			spdlog::error("Failed to load sound file at: {}", filepath);
			return;
		}

		soundBuffers[name] = std::move(buffer);
		spdlog::info("Registered Sound Effect: {}", name);
	}

	sf::SoundBuffer &getSoundBuffer(const std::string &name)
	{
		if (!soundBuffers.contains(name)) {
			spdlog::error("Sound buffer '{}' not found in AssetManager!", name);
			throw std::runtime_error("Sound buffer not found");
		}
		return soundBuffers[name];
	}

  private:
	std::shared_ptr<sf::Texture> textureSet;
	AssetManager()
	{
		textureSet = std::make_shared<sf::Texture>();
		if (!textureSet->loadFromFile(SPRITE_SHEET_PATH)) {
			throw std::runtime_error("Failed to load texture");
		}
	}
};