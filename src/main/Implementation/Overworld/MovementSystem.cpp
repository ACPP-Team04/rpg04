
#include "Abstract/Overwordl/MovementSystem.hpp"

#include "Abstract/Overwordl/Components.hpp"

MovementSystem::MovementSystem(ArchetypeManager &manager) : System(manager) {}
static float SPEED = 0.1f;
void MovementSystem::update()
{
	this->manager.view<InputComponent, TransformComponent>().each(
	    [this](EntityID id, InputComponent &input, TransformComponent &transform) {
		    if (input.moveRight.pressed) {
			    transform.position.x += SPEED;
		    }
		    if (input.moveLeft.pressed) {
			    transform.position.x -= SPEED;
		    }
		    if (input.moveUp.pressed) {
			    transform.position.y -= SPEED;
		    }
		    if (input.moveDown.pressed) {
			    transform.position.y += SPEED;
		    }
	    });
}