
#include "Abstract/Overwordl/InteractionSystem.hpp"

#include "Abstract/Overwordl/CollisionSystem.hpp"
#include "Abstract/Overwordl/Components.hpp"
InteractionSystem::InteractionSystem(ArchetypeManager&manager):System(manager)
{
}

void InteractionSystem::update()
{

	std::vector<EntityID> entitiesWithCollision;
	this->manager.view<InteractionComponent>().each([&](EntityID entity, InteractionComponent & icomp) {
		entitiesWithCollision.push_back(entity);
	});

	EntityID player;
	this->manager.view<CurrentLayerComponent>().each([&](EntityID entity, CurrentLayerComponent & icomp) {
		player = entity;
	});

	auto &spriteA = manager.getComponent<SpriteComponent>(player);
	auto &transformA = manager.getComponent<TransformComponent>(player);

	for (EntityID interactionEntity : entitiesWithCollision) {
		auto &spriteB = manager.getComponent<SpriteComponent>(interactionEntity);
		auto &transformB = manager.getComponent<TransformComponent>(interactionEntity);

		auto a = getSptiteWithPostion(spriteA, transformA);
		auto b = getSptiteWithPostion(spriteB, transformB);
		bool collides = isColliding(a,b);

		if (collides) {
			manager.getComponent<InteractionComponent>(interactionEntity).isActive = true;
		}
		else {
			manager.getComponent<InteractionComponent>(interactionEntity).isActive = false;
		}
	}

}