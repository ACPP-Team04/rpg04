#pragma once
#include "Abstract/ECS/System/System.hpp"
#include <Implementation/Components/BattleComponent.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class BattleInputSystem : public System {
  public:
	BattleInputSystem(ArchetypeManager &manager, EntityID playerId, tgui::Gui &gui)
	    : System(manager), playerId(playerId), gui(gui)
	{
		setupUI();
	}

	void update() override;

  private:
	tgui::Gui &gui;
	EntityID playerId;

	void setupUI()
	{
		auto panel = tgui::Panel::create({"200", "150"});
		panel->setPosition("5%", "70%");

		auto btn = tgui::Button::create("Attack");
		btn->onPress([this]() {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.battleState = BattleState::WAITING_FOR_INPUT;
			std::cout << "Attack button pressed!" << std::endl;
		});

		panel->add(btn);
		gui.add(panel, "BattleMenu");
	}
};
