#include "Abstract/Persistance/SaveManager.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Exception/SaveSlotNotFoundException.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include <gtest/gtest.h>
class SaveManagerTest : public ::testing::Test {
  protected:
	ArchetypeManager manager;
	const int TEST_SLOT = 999;

	void SetUp() override
	{
		SaveManager::deleteSave(TEST_SLOT);
		PersistenceManager::getInstance().clearAll();
	}

	void TearDown() override
	{
		if (SaveManager::doesSaveExist(TEST_SLOT)) {
			SaveManager::deleteSave(TEST_SLOT);
		}
		PersistenceManager::getInstance().clearAll();
	}
};

TEST_F(SaveManagerTest, ThrowsExceptionWhenSlotDoesNotExist)
{
	EXPECT_FALSE(SaveManager::doesSaveExist(TEST_SLOT));
	EXPECT_THROW({ SaveManager::loadSaveFile(TEST_SLOT); }, SaveSlotNotFoundException);
}

TEST_F(SaveManagerTest, PlayerStatsRoundTripSerialization)
{
	EntityID originalPlayer = manager.createEntity<PlayerComponent, TransformComponent, CharacterComponent>();
	auto &origTrans = manager.getComponent<TransformComponent>(originalPlayer);
	origTrans.position = {150.5f, 300.0f};

	auto &origChar = manager.getComponent<CharacterComponent>(originalPlayer);
	origChar.stats.health = 42;
	origChar.stats.experience = 1337;
	origChar.stats.addScalableStats(STRENGTH, 15);

	SaveManager::saveGame(manager, TEST_SLOT);

	EXPECT_TRUE(SaveManager::doesSaveExist(TEST_SLOT));

	nlohmann::json loadedData = SaveManager::loadSaveFile(TEST_SLOT);

	EntityID loadedPlayer = manager.createEntity<PlayerComponent, TransformComponent, CharacterComponent>();

	SaveManager::injectPlayer(manager, loadedData["player"], loadedPlayer);

	auto &loadedTrans = manager.getComponent<TransformComponent>(loadedPlayer);
	EXPECT_FLOAT_EQ(150.5f, loadedTrans.position.x);
	EXPECT_FLOAT_EQ(300.0f, loadedTrans.position.y);

	auto &loadedChar = manager.getComponent<CharacterComponent>(loadedPlayer);
	EXPECT_EQ(42, loadedChar.stats.health);
	EXPECT_EQ(1337, loadedChar.stats.experience);
	EXPECT_EQ(15, loadedChar.stats.getStat(STRENGTH));
}