#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void RenderSystem::update()
{
	this->manager.view<RenderComponent, TransformComponent>().each(
	    [this](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp) {
	    	sf::Texture tex = AssetManager::getInstance().getSpriteAt(comp.activeTiles);
		    sf::Sprite sp = sf::Sprite(tex);
		    sp.setPosition(tcomp.position);
		    sp.setScale(tcomp.scale);
		    sp.setRotation(tcomp.rotation);
		    window.draw(sp);
	    });
}