#pragma once
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/UI/BattleUI.hpp"
#include "Abstract/ECS/System/System.hpp"
#include <Implementation/Components/BattleComponent.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class BattleInputSystem : public System {
  private:
	BattleUI ui;
	void connectCallbacks();

  public:
	BattleInputSystem(ArchetypeManager &manager, tgui::Gui &gui);

	void update() override;
	void init();
	EntityID selectTarget(std::vector<EntityID> participants, EntityID playerId);
};
