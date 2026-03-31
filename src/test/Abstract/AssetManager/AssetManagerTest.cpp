#include <gtest/gtest.h>
#include "Abstract/GlobalProperties.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/AssetManager/AssetManager.hpp"


TEST(AssetManager,TestSingleton)
{
	AssetManager& instance = AssetManager::getInstance();
	AssetManager& instance2 = AssetManager::getInstance();

	EXPECT_EQ(&instance, &instance2);
}
TEST(AssetManagerTest,LoadSprite)
{
	AssetManager& instance = AssetManager::getInstance();
	auto expectedInfo = TILE_DICT.at(PLAYER_IDLE_FRONT);
	auto sprite = instance.getSpriteAt(expectedInfo);
	sf::FloatRect globBounds = sprite.getGlobalBounds();

	EXPECT_EQ(globBounds.size.x, expectedInfo.width);
	EXPECT_EQ(globBounds.size.y, expectedInfo.height);
}