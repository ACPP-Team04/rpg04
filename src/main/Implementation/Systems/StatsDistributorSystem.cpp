#include "Abstract/Combat/Systems/StatsDistributorSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
StatsDistributorSystem::StatsDistributorSystem(ArchetypeManager &manager, tgui::Gui &gui) : System(manager), gui(gui) {}

void StatsDistributorSystem::update()
{
	auto players = manager.getEntityIdByTag(EntityTag::PLAYER);
	if (players.empty())
		return;
	EntityID playerId = players[0];
	auto &battle = manager.getComponent<BattleComponent>(playerId);

	if (battle.battleState == BattleState::STATS_DISTRIBUTION) {
		if (!isMenuOpen) {
			showLevelUpMenu(playerId, battle);
			isMenuOpen = true;
		}
	}
}

void StatsDistributorSystem::showLevelUpMenu(EntityID playerId, BattleComponent &battle)
{
	auto &stats = manager.getComponent<StatsComponent>(playerId);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle.battleManagerId);
	struct LevelUpData {
		int points;
		int extraStr = 0;
		int extraDex = 0;
		int extraFaith = 0;
		tgui::Label::Ptr mainLabel;
	};
	auto data = std::make_shared<LevelUpData>();
	data->points = bmc.playerXpReward;

	auto window = tgui::ChildWindow::create("Level Up!");
	window->setTitleButtons(tgui::ChildWindow::TitleButton::None);
	window->setSize(400, 300);
	window->setOrigin(0.5f, 0.5f);
	window->setPosition("50%", "50%");
	gui.add(window);

	data->mainLabel = tgui::Label::create("Points left: " + std::to_string(data->points));
	data->mainLabel->setPosition(20, 20);
	window->add(data->mainLabel);

	auto addStatRow = [window, data](std::string name, int &tempValue, int yPos) {
		auto label = tgui::Label::create(name + ": +0");
		label->setPosition(20, yPos);
		window->add(label);

		auto btnPlus = tgui::Button::create("+");
		btnPlus->setPosition(150, yPos);
		btnPlus->setSize(30, 30);

		auto btnMinus = tgui::Button::create("-");
		btnMinus->setPosition(190, yPos);
		btnMinus->setSize(30, 30);

		btnPlus->onPress([data, &tempValue, label, name]() {
			if (data->points > 0) {
				data->points--;
				tempValue++;
				label->setText(name + ": +" + std::to_string(tempValue));
				data->mainLabel->setText("Points left: " + std::to_string(data->points));
			}
		});

		btnMinus->onPress([data, &tempValue, label, name]() {
			if (tempValue > 0) {
				data->points++;
				tempValue--;
				label->setText(name + ": +" + std::to_string(tempValue));
				data->mainLabel->setText("Points left: " + std::to_string(data->points));
			}
		});

		window->add(btnPlus);
		window->add(btnMinus);
	};
	addStatRow("Strength", data->extraStr, 60);
	addStatRow("Dexterity", data->extraDex, 100);
	addStatRow("Faith", data->extraFaith, 140);

	auto confirmBtn = tgui::Button::create("Save and continue");
	confirmBtn->setPosition(100, 220);

	confirmBtn->onPress([this, &battle, &stats, data, window]() {
		stats.strength += data->extraStr;
		stats.dexterity += data->extraDex;
		stats.faith += data->extraFaith;

		battle.battleState = BattleState::VICTORY;
		gui.remove(window);
		this->isMenuOpen = false;
	});
	window->add(confirmBtn);
}
