#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void RenderSystem::update()
{
	this->manager.view<RenderComponent, TransformComponent>().each(
	    [this](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp) {
		    sf::Sprite sp = AssetManager::getInstance().getSpriteAt(comp.activeTiles);
		    sp.setPosition(tcomp.position);
		    sp.setScale(tcomp.scale);
		    sp.setRotation(tcomp.rotation);
		    window.draw(sp);
	    });
}