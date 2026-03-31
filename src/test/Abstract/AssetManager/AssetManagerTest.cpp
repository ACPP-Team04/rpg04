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
