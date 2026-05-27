#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"

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
	sp.setPosition(tcomp.position);
	sp.setRotation(tcomp.rotation);
	window.draw(sp);
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
	for (auto& tileLayer : world->worlds[world->currentGroup].tileLayers) {
		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				auto &tile = tileLayer.tiles[y][x];
				if (tile.tileInfo.tilesetPath.empty()) continue;
				sf::Sprite sp = AssetManager::getInstance().getSpriteAt(tile.tileInfo);
				sp.setPosition(tile.position);
				window.draw(sp);
			}
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
}