#include "Abstract/Overwordl/BoundingBoxSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/Components/BoundingBoxComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <SFML/Graphics/Sprite.hpp>

sf::Sprite getSpriteWithPosition(SpriteComponent &sprite, TransformComponent &transform)
{
	sf::Sprite sfmlSprite = AssetManager::getInstance().getSpriteAt(sprite.textureId);
	sfmlSprite.setPosition(transform.position);
	sfmlSprite.setPosition(transform.position);
	sfmlSprite.setScale(transform.scale);
	sfmlSprite.setRotation(transform.rotation);
	return sfmlSprite;
}
BoundingBoxSystem::BoundingBoxSystem(ArchetypeManager &manager) : System(manager) {}

void BoundingBoxSystem::update()
{
	this->manager.view<SpriteComponent, TransformComponent, BoundIngBoxComponent>().each(
	    [&](auto &entity, auto &sprite, auto &transform, auto &bb) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, entity)) {
			    return;
		    }
		    auto s = getSpriteWithPosition(sprite, transform);
		    bb.bounds = s.getGlobalBounds();
	    });
}