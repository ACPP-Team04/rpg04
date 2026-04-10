
#include "Abstract/Overwordl/MovementSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

MovementSystem::MovementSystem(ArchetypeManager &manager) : System(manager) {}
static float SPEED = 3.0f;
void MovementSystem::update()
{
	this->manager.view<InputComponent, TransformComponent, MovementComponent>().each(
	    [this](EntityID id, InputComponent &input, TransformComponent &transform, MovementComponent &movement) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, id)) {
			    return;
		    }
		    sf::Vector2f prevPosition = transform.position;
		    if (input.moveRight.pressed) {
			    transform.position.x += movement.speed;
		    }
		    if (input.moveLeft.pressed) {
			    transform.position.x -= movement.speed;
		    }
		    if (input.moveUp.pressed) {
			    transform.position.y -= movement.speed;
		    }
		    if (input.moveDown.pressed) {
			    transform.position.y += movement.speed;
		    }
		    transform.previousPosition = prevPosition;
	    });
}