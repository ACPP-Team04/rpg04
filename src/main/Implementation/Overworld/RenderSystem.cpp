#include "Abstract/Overwordl/RenderSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

RenderSystem::RenderSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void render(TransformComponent &tcomp, sf::RenderWindow &window,
            SpriteComponent &scomp)
{

	sf::Sprite sp = AssetManager::getInstance().getSpriteAt(scomp);	
	sp.setPosition(tcomp.position);
	sp.setScale(tcomp.scale);
	sp.setRotation(tcomp.rotation);
	window.draw(sp);
}
void RenderSystem::update()
{
	WorldUtils::viewInCurrentLayer<RenderComponent, TransformComponent, SpriteComponent>(manager,
		[&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp, SpriteComponent &scomp) {
			if (comp.z_layer == 0) {
				render(tcomp, window, scomp);
			}
		});

	WorldUtils::viewInCurrentLayer<RenderComponent, TransformComponent, SpriteComponent>(manager,
		[&](const EntityID &id, RenderComponent &comp, TransformComponent &tcomp, SpriteComponent &scomp) {
			if (comp.z_layer) {
				render(tcomp, window, scomp);
			}
		});
}