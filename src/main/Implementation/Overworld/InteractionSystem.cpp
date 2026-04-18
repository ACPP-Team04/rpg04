
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include "Abstract/MathUtils.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <magic_enum/magic_enum.hpp>

#include <cfloat>

InteractionSystem::InteractionSystem(ArchetypeManager &manager) : System(manager) {}

void InteractionSystem::update()
{
	EntityID player = WorldUtils::getPlayer(manager).value();
	EntityID nearestInteractionEntity;
	float smallestDistance = FLT_MAX;
	bool candidateFound = false;
	WorldUtils::viewInCurrentLayer<InteractionComponent, BoundIngBoxComponent>(
	    manager, [&](auto &interactableEntity, InteractionComponent &component, BoundIngBoxComponent &bb) {
		    component.inRange = false;

		    auto &playerBB = manager.getComponent<BoundIngBoxComponent>(player);
		    if (!isinRadius(playerBB, bb, component.focusRadius)) {
			    component.isActive = false;
			    component.mustLeaveRadius = false;
			    return;
		    }
		    if (component.mustLeaveRadius)
			    return;
		    float newCandidateDistance = distance(playerBB.bounds, bb.bounds);
		    if (distance(playerBB.bounds, bb.bounds) < smallestDistance) {
			    smallestDistance = newCandidateDistance;
			    nearestInteractionEntity = interactableEntity;
			    candidateFound = true;
		    }
	    });

	if (!candidateFound)
		return;

	auto &component = manager.getComponent<InteractionComponent>(nearestInteractionEntity);

	if (component.trigger == INTERACTION_TRIGGER::onEnter) {
		component.isActive = true;
		return;
	}

	if (component.trigger == INTERACTION_TRIGGER::byInteractionKey) {
		if (!manager.hasComponent<InputComponent>(player)) {
			return;
		}
		auto &inputComponent = manager.getComponent<InputComponent>(player);
		component.inRange = true;
		if (inputComponent.interact.justPressed) {
			component.isActive = true;
		}
		if (component.inRange && !component.isActive) {
			std::cout << magic_enum::enum_name(component.interactionKey) << std::endl;
			return;
		}
	}
}
