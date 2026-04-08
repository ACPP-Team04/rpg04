
#include "Abstract/Overwordl/CollisionSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

CollisionSystem::CollisionSystem(ArchetypeManager &manager) : System(manager) {}

sf::Sprite getSptiteWithPostion(SpriteComponent &sprite, TransformComponent &transform)
{
	sf::Sprite sfmlSprite = AssetManager::getInstance().getSpriteAt(sprite.textureId);
	sfmlSprite.setPosition(transform.position);
	sfmlSprite.setPosition(transform.position);
	sfmlSprite.setScale(transform.scale);
	sfmlSprite.setRotation(transform.rotation);
	return sfmlSprite;
}

bool isColliding(sf::Sprite &spriteA, sf::Sprite &spriteB)
{
	auto intersection = spriteA.getGlobalBounds().findIntersection(spriteB.getGlobalBounds());
	return intersection.has_value();
}

void resolveCollision(CollisionComponent &collision, TransformComponent &transform)
{
	if (collision.isStatic) {
		return;
	}
	switch (collision.action) {
	case COLLISION_ACTION::PREV_POSITION:
		transform.position = transform.previousPosition;
		break;
	default:
		return;
	}
}

void CollisionSystem::update()
{
	std::vector<EntityID> entities;
	this->manager.view<CollisionComponent, TransformComponent, SpriteComponent>().each(
	    [&](const auto &entityA, auto &collisionA, auto &transformA, auto &spriteA) { entities.push_back(entityA); });

	for (int i = 0; i < entities.size(); i++) {
		for (int j = 0; j < entities.size(); j++) {
			if (j == i) {
				continue;
			}

			auto &spriteA = manager.getComponent<SpriteComponent>(entities.at(i));
			auto &transformA = manager.getComponent<TransformComponent>(entities.at(i));
			auto &collisionA = manager.getComponent<CollisionComponent>(entities.at(i));

			auto &spriteB = manager.getComponent<SpriteComponent>(entities.at(j));
			auto &transformB = manager.getComponent<TransformComponent>(entities.at(j));
			auto &collisionB = manager.getComponent<CollisionComponent>(entities.at(j));

			auto a = getSptiteWithPostion(spriteA, transformA);
			auto b = getSptiteWithPostion(spriteB, transformB);
			bool collides = isColliding(a, b);

			if (collides) {
				resolveCollision(collisionA, transformA);
				resolveCollision(collisionB, transformB);
			}
		}
	}
}