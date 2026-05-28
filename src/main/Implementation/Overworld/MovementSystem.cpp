
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

	WorldComponent *comp = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<InputComponent, TransformComponent, MovementComponent, StateComponent>(
	    manager, [&](EntityID id, InputComponent &input, TransformComponent &transform, MovementComponent &movement,
	                 StateComponent &state_component) {
		    sf::Vector2f prevPosition = transform.position;
		    state_component.setState(ENTITY_ANIMATIONS_STATE::IDLE, false);
		    sf::Vector2f direction(0.0f, 0.0f);

		    if (input.moveRight.pressed) {
			    direction.x += 1.0f;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_RIGHT, false);
		    }
		    if (input.moveLeft.pressed) {
			    direction.x -= 1.0f;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_LEFT, false);
		    }
		    if (input.moveUp.pressed) {
			    direction.y -= 1.0f;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_UP, false);
		    }
		    if (input.moveDown.pressed) {
			    direction.y += 1.0f;
			    state_component.setState(ENTITY_ANIMATIONS_STATE::WALK_DOWN, false);
		    }

		    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		    if (length > 0.0f) {
			    direction.x /= length;
			    direction.y /= length;
		    }
		    transform.position.x += direction.x * movement.speed;
		    transform.position.y += direction.y * movement.speed;
		    float maxWidth = comp->width * comp->tileWidth - transform.size.x;
		    float maxHeight = comp->height * comp->tileHeight - transform.size.y;

		    transform.position.x = std::clamp(transform.position.x, 0.f, maxWidth);
		    transform.position.y = std::clamp(transform.position.y, 0.f, maxHeight);
		    transform.previousPosition = prevPosition;
	    });
}