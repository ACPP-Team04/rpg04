#include "Abstract/ECS/Component.hpp"
#include "Abstract/ECS/Entity.hpp"
#include "Abstract/ECS/Test1.hpp"
#include <Abstract/ECS/SlowRepository.hpp>

#include <gtest/gtest.h>

TEST(PlayerTest, NameIsCorrect)
{
	auto* repository = new SlowRepository();

	Entity entity(23);
	repository->addComponentToEntity<Test1>(entity);

	std::optional<std::shared_ptr<Test1>> component = repository->getComponentByEntity<Test1>(entity);

	if (!component) {
		std::cerr << "Test1 component not found!" << std::endl;
	}
	std::cout << component.value()->x<< std::endl;


}