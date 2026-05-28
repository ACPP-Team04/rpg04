
#include "Abstract/Overwordl/AnimationSetterSystem.hpp"

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
AnimationSetterSystem::AnimationSetterSystem(ArchetypeManager &manager) : System(manager) {};

void AnimationSetterSystem::update()
{

	std::cout << "AnimationSetterSystem::update" << std::endl;
	WorldUtils::viewInCurrentLayer<AnimationComponent, SpriteComponent>(
		manager, [&](EntityID& entity, AnimationComponent &component, SpriteComponent &sprite) {
			if (!component.getCurrentAnimation().has_value()) {
				return;
			}

			auto &animSprite = manager.getComponent<SpriteComponent>(component.getCurrentAnimation().value());
			sprite.tilesetPath = animSprite.tilesetPath;
			sprite.tileInfo = animSprite.tileInfo;
		});
}