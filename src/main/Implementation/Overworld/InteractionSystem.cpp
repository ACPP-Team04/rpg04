
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
	auto playerBB = manager.getComponent<TransformComponent>(player).getBoundingBox();
	float smallestDistance = FLT_MAX;
	EntityID nearestInteractionEntity;
	bool candidateFound = false;
	WorldUtils::viewInCurrentLayer<InteractionComponent, TransformComponent>(
	    manager, [&](auto &interactableEntity, InteractionComponent &component, TransformComponent &bb) {
		    component.inRange = false;
		    float triggerPadding = 5.0f;
            if (component.action == INTERACTION_ACTION::START_BATTLE){
                triggerPadding = 6.5f;
            }
		    auto interActableBB = bb.getBoundingBox(triggerPadding, triggerPadding);
		    if (!collides(playerBB, interActableBB)) {
			    component.isActive = false;
			    component.mustLeaveRadius = false;
			    return;
		    }
		    if (component.mustLeaveRadius)
			    return;
		    float newCandidateDistance = distance(playerBB, interActableBB);
		    if (distance(playerBB, interActableBB) < smallestDistance) {
			    smallestDistance = newCandidateDistance;
			    nearestInteractionEntity = interactableEntity;
			    candidateFound = true;
		    }
	    });

	if (!candidateFound) {
		WorldUtils::getWorld(manager)->removePersistentMessage("Press F to interact");
		return;
	}

	auto &component = manager.getComponent<InteractionComponent>(nearestInteractionEntity);

	if (component.deactivated) {
		return;
	}
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
		if (sf::Keyboard::isKeyPressed(component.interactionKey)) {
			component.isActive = true;
		}
		WorldComponent *worldComponent = WorldUtils::getWorld(manager);
		if (component.inRange && !component.isActive) {

			worldComponent->addPersistentMessage(component.alert);
			return;
		}
		worldComponent->removePersistentMessage(component.alert);
	}
}
