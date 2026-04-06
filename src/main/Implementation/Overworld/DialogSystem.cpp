#include "Abstract/Overwordl/DialogSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"
#include "Abstract/Overwordl/Components.hpp"


#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

DialogSystem::DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {}

void DialogSystem::update()
{

	InputComponent *input;
	this->manager.view<InputComponent>().each([&](EntityID id, auto &inputComponent) {
		input = &inputComponent;
	});

	sf::Font font;
	font.openFromFile(FONT);
	sf::Text text(font);
	this->manager.view<InteractionComponent,NPC_Component,DialogComponent,TransformComponent,RenderComponent,SpriteComponent>().each([&](auto &entity,auto &interactioncomp,auto &npccomponent, DialogComponent &component, auto &transform, auto &render, auto &sprite) {

		if (!interactioncomp.isActive) {
			component.isActive = false;
			return;
		}

		std::string dialog;
		if (input->interact.justPressed) {
			component.isActive = true;
			component.nextSentence();
		}

		if (component.isActive) {
			dialog = component.currentSentence;
		}

		text.setString(dialog);
		text.setCharacterSize(component.characterSize);
		text.setFillColor(component.color);
		text.setPosition(transform.position);
		window.draw(text);


	});
}