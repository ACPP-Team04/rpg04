#pragma once
#include "Abstract/GlobalProperties.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/TILE_ENUMS.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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

	std::unordered_map<std::string, std::shared_ptr<sf::Texture>> chacheImage = {};

	sf::Sprite getSpriteAt(SpriteComponent& tile)
	{
		if (!chacheImage.contains(tile.tilesetPath)) {
			if (!textureSet->loadFromFile(tile.tilesetPath)) {
				throw std::runtime_error("Failed to load texture");
			}
			chacheImage[tile.tilesetPath] = textureSet;
		}
		return {*chacheImage[tile.tilesetPath], sf::IntRect({tile.tileInfo.pixelX, tile.tileInfo.pixelY}, {tile.tileInfo.width, tile.tileInfo.height})};
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