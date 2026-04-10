#include "Abstract/Overwordl/DialogSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

DialogSystem::DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {}

void DialogSystem::update()
{
	InputComponent &input = WorldUtils::getPlayersComponent<InputComponent>(manager).value();

	sf::Font font;
	font.openFromFile(FONT);
	sf::Text text(font);
	WorldUtils::viewInCurrentLayer<InteractionComponent, NPC_Component, DialogComponent, TransformComponent, RenderComponent,
	          SpriteComponent>(manager,[&](auto &entity, InteractionComponent &interactioncomp, auto &npccomponent, DialogComponent &dialogComp,
	              auto &transform, auto &render, auto &sprite) {
		    if (!interactioncomp.isActive) {
			    dialogComp.isActive = false;
			    return;
		    }
		    if (!dialogComp.isActive) {
			    dialogComp.isActive = true;
		    }
		    if (input.interact.justPressed) {
			    dialogComp.nextSentence();
		    }
		    text.setString(dialogComp.currentSentence);
		    text.setCharacterSize(dialogComp.characterSize);
		    text.setFillColor(dialogComp.color);
		    text.setPosition(transform.position);
		    window.draw(text);
	    });
}