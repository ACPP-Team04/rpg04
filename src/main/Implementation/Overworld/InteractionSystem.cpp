
#include "Abstract/Overwordl/InteractionSystem.hpp"
#include <magic_enum/magic_enum.hpp>
#include "Abstract/MathUtils.hpp"
#include "Abstract/Overwordl/Components.hpp"

#include <cfloat>

InteractionSystem::InteractionSystem(ArchetypeManager &manager) : System(manager) {}


void InteractionSystem::update()
{
	EntityID *player;
	bool playerFound = false;
	this->manager.view<PlayerComponent, InputComponent, CollisionComponent, BoundIngBoxComponent>()
		.each([&](auto& entity, auto& component, auto& input, auto& collision, auto& bounds) {
			player = &entity;
			playerFound = true;
		});

	if (!playerFound) return;

	EntityID nearestInteractionEntity;
	float smallestDistance = FLT_MAX;
	bool candidateFound = false;
	this->manager.view<InteractionComponent, BoundIngBoxComponent>()
		.each([&](auto& interactableEntity, InteractionComponent& component, BoundIngBoxComponent& bb) {
			component.inRange = false;

			auto& playerBB = manager.getComponent<BoundIngBoxComponent>(*player);
			if (!isinRadius(playerBB, bb, component.focusRadius)) {
				component.isActive = false;
				return;
			}
			float newCandidateDistance = distance(playerBB.bounds,bb.bounds);
			if (distance(playerBB.bounds,bb.bounds) < smallestDistance) {
				smallestDistance = newCandidateDistance;
				nearestInteractionEntity = interactableEntity;
				candidateFound = true;
			}

		});

	if (!candidateFound) return;

	auto& component = manager.getComponent<InteractionComponent>(nearestInteractionEntity);

	if (component.trigger == INTERACTION_TRIGGER::onEnter) {
		component.isActive = true;
		return;
	}

	if (component.trigger == INTERACTION_TRIGGER::byInteractionKey) {
		auto& inputComponent = manager.getComponent<InputComponent>(*player);
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
