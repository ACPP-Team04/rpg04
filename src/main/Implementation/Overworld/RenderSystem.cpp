#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void RenderSystem::update()
{
	CurrentLayerComponent currentLayer;
	this->manager.view<CurrentLayerComponent>().each([&](const EntityID& id, auto& component) {
		currentLayer = component;
	});
	this->manager.view<PartOfLayerComponent,RenderComponent, TransformComponent>().each(
	    [&](const EntityID &id,PartOfLayerComponent &partComp, RenderComponent &comp, TransformComponent &tcomp) {

	    	if (currentLayer.layer != partComp.layer || currentLayer.level != partComp.level) {
	    		std::cout << currentLayer.layer << " " << currentLayer.level << std::endl;
	    		return;
	    	}
		    sf::Sprite sp = AssetManager::getInstance().getSpriteAt(comp.activeTiles);
		    sp.setPosition(tcomp.position);
		    sp.setScale(tcomp.scale);
		    sp.setRotation(tcomp.rotation);
		    window.draw(sp);
	    });
}