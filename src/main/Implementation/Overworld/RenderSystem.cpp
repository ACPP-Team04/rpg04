#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"

#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"


RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void render(TransformComponent &tcomp, sf::RenderWindow &window, SpriteComponent &scomp)
{
	sf::Sprite sp = AssetManager::getInstance().getSpriteAt(scomp);
	sp.setScale(tcomp.scale);
	sp.setRotation(tcomp.rotation);
	auto bounds = sp.getLocalBounds();
	sp.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
	sp.setPosition(tcomp.position + sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
	window.draw(sp);
}

void renderTileLayer(int startX, int startY, int endY, int endX, sf::RenderWindow &window, TileLayer &tileLayer)
{
	for (int y = startY; y < endY; y++) {
		for (int x = startX; x < endX; x++) {
			auto& tile = tileLayer.tiles[y][x];
			if (tile.tileInfo.tilesetPath.empty()) continue;
			sf::Sprite sp = AssetManager::getInstance().getSpriteAt(tile.tileInfo);
			sp.setPosition(tile.position);
			window.draw(sp);
		}
	}
}
void RenderSystem::renderTiles(WorldComponent *world)
{
	sf::View view = window.getView();
	sf::Vector2f center = view.getCenter();
	sf::Vector2f size   = view.getSize();
	int startX = std::max(0, (int)((center.x - size.x / 2) / world->tileWidth));
	int startY = std::max(0, (int)((center.y - size.y / 2) / world->tileHeight));
	int endX = std::min((int)world->width,  startX + (int)(size.x / world->tileWidth) + 2);
	int endY = std::min((int)world->height, startY + (int)(size.y / world->tileHeight) + 2);

	auto& group = world->worlds[world->currentGroup];

	for (size_t i = 0; i < group.tileLayers.size(); i++) {
		if (i < group.tileLayers.size()) {
			renderTileLayer(startX, startY, endY, endX, window, group.tileLayers[i]);
		}
	}
	for (size_t i = 0; i < group.collidingTileLayers.size(); i++) {
		if (i < group.collidingTileLayers.size()) {
			renderTileLayer(startX, startY, endY, endX, window, group.collidingTileLayers[i]);
		}
	}
}
void RenderSystem::update()
{

	WorldComponent *world = WorldUtils::getWorld(manager);

	renderTiles(world);

	WorldUtils::viewInCurrentLayer<RenderComponent, TransformComponent, SpriteComponent>(
	    manager, [&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp, SpriteComponent &scomp) {
	    	render(tcomp, window, scomp);
	    });

	if (PEROMANCE_TEST_MODE) {
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color::Transparent);

		rect.setOutlineColor(sf::Color::Green);
		rect.setOutlineThickness(1.f);
		WorldUtils::viewInCurrentLayer<TransformComponent, CollisionComponent>(
			manager, [&](const EntityID &id, TransformComponent &tcomp, CollisionComponent &col) {
				auto bb = tcomp.getBoundingBox();
				rect.setPosition(bb.position);
				rect.setSize(bb.size);
				window.draw(rect);
			});

		rect.setOutlineColor(sf::Color::Red);
		auto& group = world->worlds[world->currentGroup];
		sf::View view = window.getView();
		sf::Vector2f center = view.getCenter();
		sf::Vector2f size   = view.getSize();
		int startX = std::max(0, (int)((center.x - size.x / 2) / world->tileWidth));
		int startY = std::max(0, (int)((center.y - size.y / 2) / world->tileHeight));
		int endX = std::min((int)world->width,  startX + (int)(size.x / world->tileWidth) + 2);
		int endY = std::min((int)world->height, startY + (int)(size.y / world->tileHeight) + 2);
		for (auto& tileLayer : group.collidingTileLayers) {
			for (int y = startY; y < endY; y++) {
				for (int x = startX; x < endX; x++) {
					if (y < 0 || y >= (int)tileLayer.tiles.size()) continue;
					if (x < 0 || x >= (int)tileLayer.tiles[y].size()) continue;
					if (tileLayer.tiles[y][x].tileInfo.width != 0) {
						rect.setPosition(sf::Vector2f(float(x * world->tileWidth), float(y * world->tileHeight)));
						rect.setSize(sf::Vector2f(float(world->tileWidth), float(world->tileHeight)));
						window.draw(rect);
					}
				}
			}
		}
	}
}