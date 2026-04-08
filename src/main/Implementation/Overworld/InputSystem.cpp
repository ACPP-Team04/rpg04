
#include "Abstract/Overwordl/InputSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"

InputSystem::InputSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager) {}
auto updateKey = [](KeyState &key, bool isDown) {
	key.justPressed = isDown && !key.pressed;
	key.pressed = isDown;
};
void InputSystem::update()
{
	manager.view<InputComponent>().each([this](const EntityID &id, InputComponent &comp) {
		updateKey(comp.moveLeft, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A));
		updateKey(comp.moveRight, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
		updateKey(comp.moveDown, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));
		updateKey(comp.moveUp, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W));
		updateKey(comp.interact, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F));
		updateKey(comp.menuButton, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape));
	});
}