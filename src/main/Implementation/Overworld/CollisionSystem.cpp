
#include "Abstract/Overwordl/CollisionSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/MathUtils.hpp"

#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

CollisionSystem::CollisionSystem(ArchetypeManager &manager) : System(manager) {}

void resolveCollision(CollisionComponent &collision, TransformComponent &transform,sf::FloatRect bounds)
{
	float dx = transform.position.x - transform.previousPosition.x;
	float dy = transform.position.y - transform.previousPosition.y;
	switch (collision.action) {
	case COLLISION_ACTION::PREV_POSITION: {
		if (dx > 0.f) {
			transform.position.x -= bounds.size.x;
		}
		else if (dx < 0.f) {
			transform.position.x += bounds.size.x;
		}
		if (dy > 0.f) {
			transform.position.y -= bounds.size.y;
		}
		else if (dy < 0.f) {
			transform.position.y += bounds.size.y;
		}
		break;
	}
	default:
		return;
	}
}
void checkCollisionWithTiles(ArchetypeManager &manager, WorldComponent &comp, CollisionComponent &collision,
                             TransformComponent &transform_component)
{
	auto &world = comp.worlds[comp.currentGroup];
	int tileWidth = comp.tileWidth;
	int tileHeight = comp.tileHeight;
	auto bb = transform_component.getBoundingBox();
	int tileX1 = (int)(bb.position.x / tileWidth);
	int tileY1 = (int)(bb.position.y / tileHeight);
	int tileX2 = (int)((bb.position.x + bb.size.x) / tileWidth);
	int tileY2 = (int)((bb.position.y + bb.size.y) / tileHeight);

	auto &tileLayers = comp.worlds[comp.currentGroup].collidingTileLayers;
	for (TileLayer &tileLayer : tileLayers) {
		for (int y = tileY1; y <= tileY2; y++) {
			for (int x = tileX1; x <= tileX2; x++) {
				if (y < 0 || y >= (int)tileLayer.tiles.size()) continue;
				if (x < 0 || x >= (int)tileLayer.tiles[y].size()) continue;
				if (tileLayer.tiles[y][x].tileInfo.width != 0) {
					sf::FloatRect tileRect(
							sf::Vector2f(float(x * tileWidth), float(y * tileHeight)),
							sf::Vector2f(float(tileWidth), float(tileHeight))
						);
					auto currentBB = transform_component.getBoundingBox();
					auto intersection = currentBB.findIntersection(tileRect);

					if (intersection.has_value()) {
						resolveCollision(collision, transform_component, intersection.value());
					}
					break;
				}
			}
		}

	}
}


void CollisionSystem::update()
{
	WorldComponent *world = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<TransformComponent, CollisionComponent>(
	    manager, [&](EntityID &id, TransformComponent &comp, CollisionComponent &collision) {
		    checkCollisionWithTiles(manager, *world, collision, comp);
	    });

	std::vector<EntityID> entities;
	WorldUtils::viewInCurrentLayer<CollisionComponent, TransformComponent>(
	    manager,
	    [&](const auto &entityA, auto &collisionAm, TransformComponent &tcomp) { entities.push_back(entityA); });

	for (int i = 0; i < (int)entities.size(); i++) {
		for (int j = i + 1; j < (int)entities.size(); j++) {
			auto &transformA = manager.getComponent<TransformComponent>(entities[i]);
			auto &collisionA = manager.getComponent<CollisionComponent>(entities[i]);
			auto &transformB = manager.getComponent<TransformComponent>(entities[j]);
			auto &collisionB = manager.getComponent<CollisionComponent>(entities[j]);

			if (collisionA.action == COLLISION_ACTION::KEEP_POSITION
			    && collisionB.action == COLLISION_ACTION::KEEP_POSITION) {
				continue;
			}

			auto bbA = transformA.getBoundingBox();
			auto bbB = transformB.getBoundingBox();
			auto intersection = bbA.findIntersection(bbB);
			if (intersection.has_value()) {
				resolveCollision(collisionA, transformA, intersection.value());
				resolveCollision(collisionB, transformB, intersection.value());
			}
		}
	}
}