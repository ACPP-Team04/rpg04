#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/PanelListBox.hpp>
struct DialogSystem : System {

	sf::RenderWindow &window;
	tgui::Gui &gui;
	DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window,tgui::Gui &gui);
	EntityID activeDialogEntity;
	bool hasActiveDialog = false;

	void update() override;

};