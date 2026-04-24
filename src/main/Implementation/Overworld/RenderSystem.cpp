#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void render(TransformComponent &tcomp, sf::RenderWindow &window, SpriteComponent &scomp)
{

	sf::Sprite sp = AssetManager::getInstance().getSpriteAt(scomp);
	sp.setPosition(tcomp.position);
	sp.setScale(tcomp.scale);
	sp.setRotation(tcomp.rotation);
	window.draw(sp);
}
void RenderSystem::update()
{
	auto renderLogic = [&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp,
	                       SpriteComponent &scomp) {
		if (manager.hasComponent<DeathComponent>(id)) {
			/*
			// DEBUG
			sf::RectangleShape debugBox;
			debugBox.setSize({(float)scomp.tileInfo.width, (float)scomp.tileInfo.height});
			debugBox.setPosition(tcomp.position);
			debugBox.setFillColor(sf::Color::Red);
			window.draw(debugBox);
			*/
			auto &deathC = manager.getComponent<DeathComponent>(id);
			SpriteComponent graveVisuals;
			graveVisuals.tileInfo = deathC.graveTile;
			graveVisuals.tilesetPath = deathC.graveTilesetPath;
			render(tcomp, window, graveVisuals);
		} else {
			render(tcomp, window, scomp);
		}
	};

	WorldUtils::viewInCurrentLayer<RenderComponent, TransformComponent, SpriteComponent>(
	    manager, [&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp, SpriteComponent &scomp) {
		    if (comp.z_layer == 0) {
			    renderLogic(id, comp, tcomp, scomp);
		    }
	    });

	WorldUtils::viewInCurrentLayer<RenderComponent, TransformComponent, SpriteComponent>(
	    manager, [&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp, SpriteComponent &scomp) {
		    if (comp.z_layer) {
			    renderLogic(id, comp, tcomp, scomp);
		    }
	    });
}