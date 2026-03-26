#include <Implementation/ECS/ECSRepository.cpp>
#include "Abstract/ECS/Entity.hpp"
#include "Abstract/ECS/Component.hpp"
#include "Abstract/ECS/Test1.hpp"

#include <gtest/gtest.h>

TEST(PlayerTest, NameIsCorrect)
{
	auto* repository = new SlowRepository();

	Entity entity(23);


	repository->addComponentToEntity<Test1>(entity);


}