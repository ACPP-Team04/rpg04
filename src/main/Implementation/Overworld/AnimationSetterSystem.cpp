
#include "Abstract/Overwordl/AnimationSetterSystem.hpp"

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
AnimationSetterSystem::AnimationSetterSystem(ArchetypeManager &manager) : System(manager) {};

void AnimationSetterSystem::update()
{

	std::cout << "AnimationSetterSystem::update" << std::endl;
	WorldUtils::viewInCurrentLayer<AnimationComponent, SpriteComponent,TransformComponent>(
		manager, [&](EntityID& entity, AnimationComponent &component, SpriteComponent &sprite,TransformComponent &tcomp) {
			if (!component.getCurrentAnimation().has_value()) {
				return;
			}

			auto &animSprite = manager.getComponent<SpriteComponent>(component.getCurrentAnimation().value());
			sprite.tilesetPath = animSprite.tilesetPath;
			sprite.tileInfo = animSprite.tileInfo;
			auto &comp = manager.getComponent<TransformComponent>(component.getCurrentAnimation().value());
			tcomp.rotation = comp.rotation;
			tcomp.scale = comp.scale;
		});
}