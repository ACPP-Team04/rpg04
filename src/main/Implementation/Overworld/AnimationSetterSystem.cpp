
#include "Abstract/Overwordl/AnimationSetterSystem.hpp"

#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <Abstract/Combat/Components/HitFeedbackComponent.hpp>
#include <Abstract/Combat/Components/LungeComponent.hpp>
#include <Abstract/Overwordl/Components/StateComponent.hpp>
AnimationSetterSystem::AnimationSetterSystem(ArchetypeManager &manager) : System(manager) {};

void AnimationSetterSystem::update()
{

	std::cout << "AnimationSetterSystem::update" << std::endl;
	std::vector<EntityID> entitiesToCleanHit;
	std::vector<EntityID> entitiesToCleanLunge;

	WorldUtils::viewInCurrentLayer<HitFeedbackComponent, SpriteComponent, StateComponent>(
	    manager, [&](EntityID id, HitFeedbackComponent &hit, SpriteComponent &sprite, StateComponent &state) {
		    hit.framesElapsed++;

		    if (hit.framesElapsed >= hit.totalFrames) {
			    state.setState(IDLE);
			    entitiesToCleanHit.push_back(id);
		    }
	    });

	WorldUtils::viewInCurrentLayer<LungeComponent, TransformComponent>(
	    manager, [&](EntityID id, LungeComponent &lunge, TransformComponent &transform) {
		    lunge.framesElapsed++;
		    int halfFrames = lunge.totalFrames / 2;

		    if (lunge.framesElapsed >= lunge.totalFrames) {
			    transform.position = lunge.originalPosition;
			    manager.getComponent<StateComponent>(id).setState(IDLE);
			    entitiesToCleanLunge.push_back(id);
			    return;
		    }

		    sf::Vector2f start, end;
		    float progress;

		    if (lunge.framesElapsed <= halfFrames) {
			    start = lunge.originalPosition;
			    end = lunge.targetPosition;
			    progress = static_cast<float>(lunge.framesElapsed) / halfFrames;
		    } else {
			    start = lunge.targetPosition;
			    end = lunge.originalPosition;
			    progress = static_cast<float>(lunge.framesElapsed - halfFrames) / halfFrames;
		    }

		    transform.position.x = start.x + (end.x - start.x) * progress;
		    transform.position.y = start.y + (end.y - start.y) * progress;
	    });

	for (EntityID id : entitiesToCleanHit) {
		manager.removeComponentFromEntity<HitFeedbackComponent>(id);
	}
	for (EntityID id : entitiesToCleanLunge) {
		manager.removeComponentFromEntity<LungeComponent>(id);
	}

	WorldUtils::viewInCurrentLayer<AnimationComponent, SpriteComponent, TransformComponent>(
	    manager,
	    [&](EntityID &entity, AnimationComponent &component, SpriteComponent &sprite, TransformComponent &tcomp) {
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