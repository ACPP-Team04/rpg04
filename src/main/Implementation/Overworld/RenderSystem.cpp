#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};


void render(CurrentLayerComponent &currentLayer,PartOfLayerComponent partComp, TransformComponent &tcomp, sf::RenderWindow &window, SpriteComponent &scomp)
{
	if (currentLayer.layer != partComp.layer || currentLayer.level != partComp.level) {
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
	CurrentLayerComponent currentLayer;
	this->manager.view<CurrentLayerComponent>().each(
	    [&](const EntityID &id, auto &component) { currentLayer = component; });

	this->manager.view<PartOfLayerComponent, RenderComponent, TransformComponent, SpriteComponent>().each(
		[&](const EntityID &id, PartOfLayerComponent &partComp, RenderComponent &comp, TransformComponent &tcomp,
			SpriteComponent &scomp) {
			if (comp.z_layer == 0) {
				render(currentLayer,partComp,tcomp,window,scomp);
			}
		});
	this->manager.view<PartOfLayerComponent, RenderComponent, TransformComponent, SpriteComponent>().each(
		[&](const EntityID &id, PartOfLayerComponent &partComp, RenderComponent &comp, TransformComponent &tcomp,
			SpriteComponent &scomp) {
			if (comp.z_layer) {
				render(currentLayer,partComp,tcomp,window,scomp);
			}
		});
}