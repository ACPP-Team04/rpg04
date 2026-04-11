#include "Abstract/Combat/Systems/StatsDistributorSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
StatsDistributorSystem::StatsDistributorSystem(ArchetypeManager &manager, tgui::Gui &gui) : System(manager), gui(gui) {}

void StatsDistributorSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();
	if (view.archetypes.size() == 0) {
		return;
	}
	auto player = WorldUtils::getPlayer(manager);
	if (!player.has_value()) {
		return;
	}
	if (!manager.hasComponent<BattleComponent>(player.value())) {
		return;
	}
	EntityID playerId = player.value();
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
		std::unordered_map<STATS, int> extraStats;
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

	auto addStatRow = [window, data](std::string name, STATS statsEnum, int yPos, int stepSize = 1) {
		data->extraStats[statsEnum] = 0;
		auto label = tgui::Label::create(name + ": +0");
		label->setPosition(20, yPos);
		window->add(label);

		auto btnPlus = tgui::Button::create("+");
		btnPlus->setPosition(150, yPos);
		btnPlus->setSize(30, 30);

		auto btnMinus = tgui::Button::create("-");
		btnMinus->setPosition(190, yPos);
		btnMinus->setSize(30, 30);

		btnPlus->onPress([data, statsEnum, label, name, stepSize]() {
			if (data->points > 0) {
				data->points--;
				data->extraStats[statsEnum] += stepSize;
				label->setText(name + ": +" + std::to_string(data->extraStats[statsEnum]));
				data->mainLabel->setText("Points left: " + std::to_string(data->points));
			}
		});

		btnMinus->onPress([data, statsEnum, label, name, stepSize]() {
			if (data->extraStats[statsEnum] > 0) {
				data->points++;
				data->extraStats[statsEnum] -= stepSize;
				label->setText(name + ": +" + std::to_string(data->extraStats[statsEnum]));
				data->mainLabel->setText("Points left: " + std::to_string(data->points));
			}
		});

		window->add(btnPlus);
		window->add(btnMinus);
	};
	addStatRow("Strength", STATS::STRENGTH, 60, 1);
	addStatRow("Dexterity", STATS::DEXTERITY, 100, 1);
	addStatRow("Faith", STATS::FAITH, 140, 1);
	addStatRow("Max Health", STATS::MAX_HEALTH, 180, 10);

	auto confirmBtn = tgui::Button::create("Save and continue");
	confirmBtn->setPosition(100, 220);

	confirmBtn->onPress([this, &battle, &stats, data, window]() {
		for (const auto &[statEnum, extraPoints] : data->extraStats) {
			if (extraPoints > 0) {
				stats.stats[statEnum] = stats.getStat(statEnum) + extraPoints;
			}
		}
		battle.battleState = BattleState::VICTORY;
		gui.remove(window);
		this->isMenuOpen = false;
	});
	window->add(confirmBtn);
}
