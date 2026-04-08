#pragma once
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
class StatsDistributorSystem : public System {
	tgui::Gui &gui;
	bool isMenuOpen = false;

  public:
	StatsDistributorSystem(ArchetypeManager &manager, tgui::Gui &gui);

	void update();

  private:
	void showLevelUpMenu(EntityID playerId, BattleComponent &battle);
};
