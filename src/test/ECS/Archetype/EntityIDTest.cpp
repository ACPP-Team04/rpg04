
#include <gtest/gtest.h>
#include "Abstract/ECS/Entity/EntityID.hpp"

std::vector<EntityID> fillNEntities(int n)
{
	std::vector<EntityID> entities;
	for (int i = 0; i < n; i++) {
		entities.emplace_back();
	}
	return entities;
}

static int N = 10;
TEST(EntityID,Unique)
{
	std::vector<EntityID> entities = fillNEntities(N);

	for (int i = 0; i < N; i++) {
		EXPECT_EQ(entities[i].getId(), i);
	}
	EXPECT_EQ(entities.back().getId(), N - 1);
}

TEST(EntityID,Copy)
{
	EntityID origin = EntityID();
	EntityID copy = origin;

	EXPECT_EQ(copy.getId(), origin.getId());
	EXPECT_EQ(EntityID::IdCounter,1);
}