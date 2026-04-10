
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include "Abstract/MathUtils.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <magic_enum/magic_enum.hpp>

#include <cfloat>

InteractionSystem::InteractionSystem(ArchetypeManager &manager) : System(manager) {}

void InteractionSystem::update()
{
	EntityID *player;
	bool playerFound = false;
	this->manager.view<PlayerComponent, InputComponent, CollisionComponent, BoundIngBoxComponent>().each(
	    [&](auto &entity, auto &component, auto &input, auto &collision, auto &bounds) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, entity)) {
			    return;
		    }
		    player = &entity;
		    playerFound = true;
	    });

	if (!playerFound)
		return;

	EntityID nearestInteractionEntity;
	float smallestDistance = FLT_MAX;
	bool candidateFound = false;
	this->manager.view<InteractionComponent, BoundIngBoxComponent>().each(
	    [&](auto &interactableEntity, InteractionComponent &component, BoundIngBoxComponent &bb) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, interactableEntity)) {
			    component.inRange = false;
			    component.isActive = false;
			    component.mustLeaveRadius = false;
			    return;
		    }
		    component.inRange = false;

		    auto &playerBB = manager.getComponent<BoundIngBoxComponent>(*player);
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
		auto &inputComponent = manager.getComponent<InputComponent>(*player);
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
