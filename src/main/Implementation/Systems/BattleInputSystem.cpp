#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

void BattleInputSystem::update()
{
	auto &battle = manager.getComponent<BattleComponent>(playerId);
	auto menu = gui.get<tgui::Panel>("BattleMenu");

	if (battle.battleState == BattleState::EXECUTING_ACTION) {
		menu->setVisible(true);
	} else {
		menu->setVisible(false);
	}
}
