#include "Abstract/Combat/Systems/StatsDistributorSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"

#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
StatsDistributorSystem::StatsDistributorSystem(ArchetypeManager &manager, tgui::Gui &gui) : System(manager), gui(gui) {}

void StatsDistributorSystem::update()
{
	if (manager.view<BattleManagerComponent>().archetypes.empty()) {
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
	const auto &battle = manager.getComponent<BattleComponent>(playerId);

	if (battle.battleState == BattleState::STATS_DISTRIBUTION && !isMenuOpen) {
		showLevelUpMenu(playerId, battle);
		isMenuOpen = true;
	}
}

void StatsDistributorSystem::showLevelUpMenu(EntityID playerId, const BattleComponent &battle)
{
	const auto &stats = manager.getComponent<CharacterComponent>(playerId).stats;
	const auto &bmc = manager.getComponent<BattleManagerComponent>(battle.battleManagerId);
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

	data->mainLabel = tgui::Label::create(std::format("Points left: {}", data->points));
	data->mainLabel->setPosition(20, 20);
	window->add(data->mainLabel);

	auto addStatRow = [&window, data](std::string name, STATS statsEnum, int yPos, int currentVal, int stepSize = 1) {
		data->extraStats[statsEnum] = 0;
		auto label = tgui::Label::create(std::format("{} ({}): +0", name, currentVal));
		label->setPosition(20, yPos);
		window->add(label);

		auto btnPlus = tgui::Button::create("+");
		btnPlus->setPosition(200, yPos);
		btnPlus->setSize(30, 30);

		auto btnMinus = tgui::Button::create("-");
		btnMinus->setPosition(240, yPos);
		btnMinus->setSize(30, 30);

		btnPlus->onPress([data, statsEnum, label, name, stepSize]() {
			if (data->points > 0) {
				data->points--;
				data->extraStats[statsEnum] += stepSize;
				label->setText(std::format("{}: + {}", name, data->extraStats[statsEnum]));
				data->mainLabel->getRenderer()->setTextColor(tgui::Color::Black);
				data->mainLabel->setText(std::format("Points left: {}", data->points));
			}
		});

		btnMinus->onPress([data, statsEnum, label, name, stepSize]() {
			if (data->extraStats[statsEnum] > 0) {
				data->points++;
				data->extraStats[statsEnum] -= stepSize;
				label->setText(std::format("{}: + {}", name, data->extraStats[statsEnum]));
				data->mainLabel->getRenderer()->setTextColor(tgui::Color::Black);
				data->mainLabel->setText(std::format("Points left: {}", data->points));
			}
		});

		window->add(btnPlus);
		window->add(btnMinus);
	};
	addStatRow("Strength", STATS::STRENGTH, 60, stats.getStat(STATS::STRENGTH), 1);
	addStatRow("Dexterity", STATS::DEXTERITY, 100, stats.getStat(STATS::DEXTERITY), 1);
	addStatRow("Faith", STATS::FAITH, 140, stats.getStat(STATS::FAITH), 1);
	addStatRow("Max Health", STATS::MAX_HEALTH, 180, stats.getStat(STATS::MAX_HEALTH), 10);

	auto confirmBtn = tgui::Button::create("Save and continue");
	confirmBtn->setPosition(100, 220);

	confirmBtn->onPress([this, playerId, data, window]() {
		if (data->points > 0) {
			data->mainLabel->getRenderer()->setTextColor(tgui::Color::Red);
			data->mainLabel->setText(std::format("Please spend all points! Points left: {}", data->points));
			return;
		}

		auto &stats = manager.getComponent<CharacterComponent>(playerId).stats;
		auto &battle = manager.getComponent<BattleComponent>(playerId);

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
