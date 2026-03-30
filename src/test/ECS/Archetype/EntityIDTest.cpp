
#include "Abstract/ECS/Entity/EntityID.hpp"
#include <gtest/gtest.h>

std::vector<EntityID> fillNEntities(int n)
{
	std::vector<EntityID> entities;
	for (int i = 0; i < n; i++) {
		entities.emplace_back();
	}
	return entities;
}

static int N = 10;
TEST(EntityID, Unique)
{
	int currentCounter = EntityID::IdCounter;
	std::vector<EntityID> entities = fillNEntities(N);

	for (int i = 0; i < N; i++) {
		EXPECT_EQ(entities[i].getId(), i + currentCounter);
	}
	size_t endCounter = EntityID::IdCounter;
	EXPECT_EQ(entities.back().getId(), endCounter - 1);
}

TEST(EntityID, Copy)
{
	int currentCounter = EntityID::IdCounter;
	EntityID origin = EntityID();
	EntityID copy = origin;

	EXPECT_EQ(copy.getId(), origin.getId());
	EXPECT_EQ(EntityID::IdCounter, currentCounter + 1);
}
