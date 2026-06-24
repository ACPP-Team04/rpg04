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
	int currentTargetIndex = 0;
	bool rightKeyWasPressed = false;
	bool leftKeyWasPressed = false;
	bool enterKeyWasPressed = false;
	float lastDrawnAP = -1.0f;
	EntityID lastActiveId;

  public:
	BattleInputSystem(ArchetypeManager &manager, tgui::Gui &gui);

	void update() override;
	void init();
	std::optional<EntityID> getActiveLocalController();
	static std::vector<EntityID> getTargetsInBattle(const EntityID playerId, const EntityID managerId,
	                                                ArchetypeManager &manager);
};
