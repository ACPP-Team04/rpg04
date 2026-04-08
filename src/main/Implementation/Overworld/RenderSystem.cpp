#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void render(WorldComponent &world, PartOfLayerComponent partComp, TransformComponent &tcomp, sf::RenderWindow &window,
            SpriteComponent &scomp)
{
	if (world.currentLayer != partComp.layer || world.currentLevel != partComp.level) {
		return;
	}
	sf::Sprite sp = AssetManager::getInstance().getSpriteAt(scomp.textureId);
	sp.setPosition(tcomp.position);
	sp.setScale(tcomp.scale);
	sp.setRotation(tcomp.rotation);
	window.draw(sp);
}
void RenderSystem::update()
{
	WorldComponent world;
	this->manager.view<WorldComponent>().each([&](const EntityID &id, auto &component) { world = component; });

	this->manager.view<PartOfLayerComponent, RenderComponent, TransformComponent, SpriteComponent>().each(
	    [&](const EntityID &id, PartOfLayerComponent &partComp, RenderComponent &comp, TransformComponent &tcomp,
	        SpriteComponent &scomp) {
		    if (comp.z_layer == 0) {
			    render(world, partComp, tcomp, window, scomp);
		    }
	    });
	this->manager.view<PartOfLayerComponent, RenderComponent, TransformComponent, SpriteComponent>().each(
	    [&](const EntityID &id, PartOfLayerComponent &partComp, RenderComponent &comp, TransformComponent &tcomp,
	        SpriteComponent &scomp) {
		    if (comp.z_layer) {
			    render(world, partComp, tcomp, window, scomp);
		    }
	    });
}