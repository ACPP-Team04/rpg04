#pragma once
#include "Abstract/Overwordl/MenuSystem.hpp"

#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
MenuSystem::MenuSystem(ArchetypeManager& manager, tgui::Gui& gui):System(manager),gui(gui)
{
};

void overworldMenu(ArchetypeManager &manager, WorldComponent* world, tgui::Gui& gui)
{
	if (gui.get("overworldMenu")) return;

	auto panel = tgui::Panel::create({"300", "400"});
	panel->setWidgetName("overworldMenu");
	panel->setPosition({"50%", "50%"});
	panel->setOrigin(0.5f, 0.5f);
	panel->getRenderer()->setBackgroundColor(tgui::Color::Red);

	InventoryComponent *inventory = nullptr;
	manager.view<PlayerComponent, InventoryComponent>().each([&](auto& entity, auto& comp, auto& invComp) {
		inventory = &invComp;
	});

	float yOffset = 10.f;
	if (inventory) {
		for (auto& itemId : inventory->inventory) {

			auto label = tgui::Label::create(std::to_string(itemId.getId()));
			label->setPosition({10.f, yOffset});
			label->setTextSize(70);
			panel->add(label);
			yOffset += 30.f;
		}
	}

	gui.add(panel);
}

void openMenu(ArchetypeManager &manager, WorldComponent* world,tgui::Gui& gui)
{
	LAYERTYPE layertype = world->currentLayer;
	world->menuOpened = true;


	if (layertype == LAYERTYPE::OVERWORLD) {
		overworldMenu(manager,world,gui);
	}
}

void MenuSystem::update()
{
	WorldComponent *world = nullptr;
	this->manager.view<WorldComponent>().each([&](auto& entity, auto& component) {
		world = &component;
	});
	InputComponent *input = nullptr;
	this->manager.view<InputComponent>().each([&](auto& entity, auto& inputComponent) {
		input = &inputComponent;
	});

	if (input->menuButton.justPressed) {
		if (!world->menuOpened) {
			openMenu(manager, world, this->gui);
		} else {
			world->menuOpened = false;
			gui.removeAllWidgets();
		}
	}
}