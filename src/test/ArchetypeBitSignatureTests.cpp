#include "Abstract/ECS/Archetype/ArchetypeBitSignature.hpp"
#include "Abstract/ECS/Test1.hpp"
#include <gtest/gtest.h>

TEST(ArchetypeBitSignatureTests, DefaultConstructorIsZero)
{
	ArchetypeBitSignature sig;
	EXPECT_EQ(sig, ArchetypeBitSignature(bitset(0)));
}

TEST(ArchetypeBitSignatureTests, EqualityOperator)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test1>();
	EXPECT_EQ(sig1, sig2);
}

TEST(ArchetypeBitSignatureTests, InequalityOperator)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test2>();
	EXPECT_NE(sig1, sig2);
}

TEST(ArchetypeBitSignatureTests, GetMultipleTypesIsConsistent)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1, Test2>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test1, Test2>();
	EXPECT_EQ(sig1, sig2);
}

TEST(ArchetypeBitSignatureTests, GetIsOrderIndependent)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1, Test2>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test2, Test1>();
	EXPECT_EQ(sig1, sig2);
}

TEST(ArchetypeBitSignatureTests, GetSubsetNotEqualToSuperset)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test1, Test2>();
	EXPECT_NE(sig1, sig2);
}

TEST(ArchetypeBitSignatureTests, IntersectOverlapping)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1, Test2>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test1>();
	ArchetypeBitSignature intersection = ArchetypeBitSignature::intersect(sig1, sig2);
	EXPECT_EQ(intersection, sig2);
}

TEST(ArchetypeBitSignatureTests, IntersectNonOverlapping)
{
	ArchetypeBitSignature sig1 = ArchetypeBitSignature::get<Test1>();
	ArchetypeBitSignature sig2 = ArchetypeBitSignature::get<Test2>();
	ArchetypeBitSignature intersection = ArchetypeBitSignature::intersect(sig1, sig2);
	EXPECT_EQ(intersection, ArchetypeBitSignature());
}
