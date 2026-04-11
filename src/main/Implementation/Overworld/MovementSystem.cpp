
#include "Abstract/Overwordl/MovementSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

MovementSystem::MovementSystem(ArchetypeManager &manager) : System(manager) {}
static float SPEED = 3.0f;
void MovementSystem::update()
{
	WorldUtils::viewInCurrentLayer<InputComponent, TransformComponent, MovementComponent>(manager,
	    [&](EntityID id, InputComponent &input, TransformComponent &transform, MovementComponent &movement) {
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