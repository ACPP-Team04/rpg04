#pragma once
#include "Abstract/GlobalProperties.hpp"
#include "Abstract/TILE_ENUMS.hpp"

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

	sf::Vector2u getSize(std::vector<std::vector<TileInfo>> info)
	{
		unsigned width = 0;
		unsigned height = 0;
		for (unsigned i = 0; i < info.size(); ++i) {
			for (unsigned j = 0; j < info[i].size(); ++j) {
				width+=info[i][j].width;
				height+=info[i][j].height;
			}
		}
		return {width,height};
	}

	sf::Texture getSpriteAt(TileType type)
	{
		auto info = TILE_DICT.at(type);
		unsigned rowsize = info.size();
		unsigned colsize = info[0].size();
		sf::RenderTexture renderTexture;
		if (!renderTexture.resize(getSize(info))) {
			throw std::runtime_error("Failed to resize texture");
		}
		renderTexture.clear(sf::Color::Transparent);
		for (unsigned i = 0; i < rowsize; ++i) {
			for (unsigned j = 0; j < colsize; ++j) {
				TileInfo tile = info[i][j];
				sf::Sprite sprite = {*this->textureSet,sf::IntRect({tile.PixelX, tile.PixelY},{tile.width, tile.height})};

				sprite.setPosition({static_cast<float>(j) * tile.width,
					static_cast<float>(i) * tile.height});
				renderTexture.draw(sprite);
			}
		}
		renderTexture.display();
		return renderTexture.getTexture();
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