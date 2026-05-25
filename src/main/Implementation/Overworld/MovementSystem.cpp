
#include "Abstract/Overwordl/MovementSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

MovementSystem::MovementSystem(ArchetypeManager &manager) : System(manager) {}
static float SPEED = 3.0f;
void MovementSystem::update()
{
	WorldUtils::viewInCurrentLayer<InputComponent, TransformComponent, MovementComponent, StateComponent>(
	    manager, [&](EntityID id, InputComponent &input, TransformComponent &transform, MovementComponent &movement,
	                 StateComponent &state_component) {
		    sf::Vector2f prevPosition = transform.position;

		    state_component.setState(ENTITY_ANIMATIONS_STATE::IDLE, false);

		    if (input.moveRight.pressed) {
			    transform.position.x += movement.speed;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_RIGHT, false);
		    }
		    if (input.moveLeft.pressed) {
			    transform.position.x -= movement.speed;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_LEFT, false);
		    }
		    if (input.moveUp.pressed) {
			    transform.position.y -= movement.speed;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_UP, false);
		    }
		    if (input.moveDown.pressed) {
			    transform.position.y += movement.speed;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_DOWN, false);
		    }

		    transform.previousPosition = prevPosition;
	    });
}