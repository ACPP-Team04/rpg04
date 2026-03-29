#include "Abstract/ECS/Test1.hpp"
#include <gtest/gtest.h>

TEST(TypeInfo, SameTypeHasStableId)
{
	int id1 = TypeInfo<Test1>::id();
	int id2 = TypeInfo<Test1>::id();

	EXPECT_EQ(id1, id2);
}

TEST(TypeInfo, DifferentTypesHaveDifferentIds)
{
	int id1 = TypeInfo<Test1>::id();
	int id2 = TypeInfo<Test2>::id();

	EXPECT_NE(id1, id2);
}

TEST(TypeInfo, BitRepresentationSetsExactlyOneBit)
{
	bitset bits = TypeInfo<Test1>::bitRepr();

	EXPECT_EQ(bits.count(), 1);
}

TEST(TypeInfo, BitRepresentationMatchesTypeId)
{
	int id = TypeInfo<Test1>::id();
	bitset bits = TypeInfo<Test1>::bitRepr();

	EXPECT_TRUE(bits.test(id));
}

TEST(TypeInfo, DifferentTypesHaveDifferentBitRepresentations)
{
	bitset bits1 = TypeInfo<Test1>::bitRepr();
	bitset bits2 = TypeInfo<Test2>::bitRepr();

	EXPECT_NE(bits1, bits2);
}

TEST(TypeInfo, SameTypeHasStableBitRepresentation)
{
	bitset bits1 = TypeInfo<Test1>::bitRepr();
	bitset bits2 = TypeInfo<Test1>::bitRepr();

	EXPECT_EQ(bits1, bits2);
}
