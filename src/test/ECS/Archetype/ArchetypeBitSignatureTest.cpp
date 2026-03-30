#include "Abstract/ECS/Archetype/ArchetypeBitSignature.hpp"
#include "../test/ECS/TestComponents.hpp"
#include <gtest/gtest.h>

TEST(ArchetypeBitSignature, Unique)
{
	ArchetypeBitSignature signatureIntComp1 = ArchetypeBitSignature::get<Intcomp1>();
	ArchetypeBitSignature signatureIntComp2 = ArchetypeBitSignature::get<Intcomp2>();
	ArchetypeBitSignature signatureIntComp3 = ArchetypeBitSignature::get<Intcomp1, Intcomp2>();
	ArchetypeBitSignature signatureIntComp4 = ArchetypeBitSignature::get<Intcomp2, Intcomp3>();
	EXPECT_NE(signatureIntComp1, signatureIntComp2);
	EXPECT_NE(signatureIntComp3, signatureIntComp4);
}

TEST(ArchetypeBitSignature, Idempotent)
{
	ArchetypeBitSignature signatureIntComp1 = ArchetypeBitSignature::get<Intcomp1>();
	ArchetypeBitSignature signatureIntComp2 = ArchetypeBitSignature::get<Intcomp1>();
	ArchetypeBitSignature signatureIntComp3 = ArchetypeBitSignature::get<Intcomp1, Intcomp2>();
	ArchetypeBitSignature signatureIntComp4 = ArchetypeBitSignature::get<Intcomp2, Intcomp1>();
	ArchetypeBitSignature signatureIntComp5 = ArchetypeBitSignature::get<Intcomp2, Intcomp1, Intcomp1>();
	EXPECT_EQ(signatureIntComp1, signatureIntComp2);
	EXPECT_EQ(signatureIntComp3, signatureIntComp4);
	EXPECT_NE(signatureIntComp1, signatureIntComp3);
	EXPECT_EQ(signatureIntComp5, signatureIntComp4);
}

TEST(ArchetypeBitSignature, Intersection)
{
	ArchetypeBitSignature signatureIntComp1 = ArchetypeBitSignature::get<Intcomp1>();
	ArchetypeBitSignature signatureIntComp2 = ArchetypeBitSignature::get<Intcomp1>();
	ArchetypeBitSignature signatureIntComp3 = ArchetypeBitSignature::get<Intcomp1, Intcomp2>();
	auto intersec = ArchetypeBitSignature::intersect(signatureIntComp1, signatureIntComp2);
	EXPECT_EQ(intersec, signatureIntComp2);
	intersec = ArchetypeBitSignature::intersect(signatureIntComp1, signatureIntComp3);
	EXPECT_EQ(intersec, signatureIntComp2);
}
