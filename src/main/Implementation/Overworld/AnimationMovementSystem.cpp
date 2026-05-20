#include "Abstract/Overwordl/AnimationMovementSystem.hpp"

#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
AnimationMovementSystem::AnimationMovementSystem(ArchetypeManager &manager):System(manager){};

void AnimationMovementSystem::update()
{
	WorldUtils::viewInCurrentLayer<StateComponent,AnimationComponent>(manager,[&](EntityID id,StateComponent &state,AnimationComponent &animation) {
		animation.setCurrentAnimation(state.getState());
	});
}