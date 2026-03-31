
#include "Abstract/Overwordl/InputSystem.hpp"

#include "Abstract/Overwordl/Components.hpp"

InputSystem::InputSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager) {}

void InputSystem::update()
{
	manager.view<InputComponent>().each([this](const EntityID &id, InputComponent &comp) {
		comp.moveLeft = {sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)};
		comp.moveRight = {sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)};
		comp.moveDown = {sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)};
		comp.moveUp = {sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)};
	});
}