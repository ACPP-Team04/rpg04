
#include "Abstract/Overwordl/CollisionSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/MathUtils.hpp"

#include "Abstract/Overwordl/Components/BoundingBoxComponent.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

CollisionSystem::CollisionSystem(ArchetypeManager &manager) : System(manager) {}

void resolveCollision(CollisionComponent &collision, TransformComponent &transform, sf::FloatRect overlap)
{
	switch (collision.action) {
	case COLLISION_ACTION::PREV_POSITION: {
		transform.position = transform.previousPosition;
		break;
	}
	default:
		return;
	}
}
void CollisionSystem::update()
{
	std::vector<EntityID> entities;
	WorldUtils::viewInCurrentLayer<CollisionComponent, BoundIngBoxComponent, TransformComponent>(manager,
	    [&](const auto &entityA, auto &collisionAm, auto &bbb, auto &tcomp) {
		    entities.push_back(entityA);

	    });

	for (int i = 0; i < entities.size(); i++) {
		for (int j = 0; j < entities.size(); j++) {
			if (j == i) {
				continue;
			}
			auto &bbA = manager.getComponent<BoundIngBoxComponent>(entities.at(i));
			auto &transformA = manager.getComponent<TransformComponent>(entities.at(i));
			auto &collisionA = manager.getComponent<CollisionComponent>(entities.at(i));

			auto &bbB = manager.getComponent<BoundIngBoxComponent>(entities.at(j));
			auto &transformB = manager.getComponent<TransformComponent>(entities.at(j));
			auto &collisionB = manager.getComponent<CollisionComponent>(entities.at(j));
			if (collides(bbA, bbB)) {

				if (collisionA.action == COLLISION_ACTION::KEEP_POSITION
				    && collisionB.action == COLLISION_ACTION::KEEP_POSITION) {

					continue;
				}
			}
			if (collides(bbA, bbB)) {
				resolveCollision(collisionA, transformA, collidesOverlap(bbA.bounds, bbB.bounds));
				resolveCollision(collisionB, transformB, collidesOverlap(bbA.bounds, bbB.bounds));
			}
		}
	}
}