#include <Abstract/Player/Player.h>
#include <gtest/gtest.h>

TEST(PlayerTest, NameIsCorrect)
{
	Player player("HIero");
	EXPECT_EQ(player.getName(), "Hero");
}