#pragma once
#include "Abstract/GlobalProperties.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
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

	sf::Sprite getSpriteAt(TileInfo info)
	{
		return {*this->textureSet, sf::IntRect({info.PixelX, info.PixelY}, {info.width, info.height})};
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