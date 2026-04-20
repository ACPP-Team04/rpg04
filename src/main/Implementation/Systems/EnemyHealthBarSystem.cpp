#include "Abstract/Combat/Systems/EnemyHealthBarSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include <spdlog/spdlog.h>

EnemyHealthBarSystem::EnemyHealthBarSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window)
    : System(manager), gui(gui), window(window)
{
}

void EnemyHealthBarSystem::update()
{
	bool battleIsActive = false;
	manager.view<BattleManagerComponent>().each([&](EntityID id, auto &bmc) { battleIsActive = true; });

	if (!battleIsActive) {
		clearAllBars();
		return;
	}

	auto playerId = WorldUtils::getPlayer(manager);
	if (!playerId.has_value()) {
		throw std::runtime_error("Player entity not found in EnemyHealthBarSystem");
	}
	std::vector<EntityID> activeEnemies;
	if (!manager.hasComponent<BattleComponent>(playerId.value())) {
		clearAllBars();
		return;
	}
	std::optional<EntityID> hoveringTargetOpt = manager.getComponent<BattleComponent>(playerId.value()).hoveringTarget;
	manager.view<BattleManagerComponent>().each([&](EntityID bmcId, BattleManagerComponent &bmc) {
		for (EntityID id : bmc.participants) {
			if (id == playerId.value()) {
				continue;
			}

			activeEnemies.push_back(id);

			auto &stats = manager.getComponent<StatsComponent>(id);
			auto &transform = manager.getComponent<TransformComponent>(id);

			sf::Vector2i pixelPos = window.mapCoordsToPixel(transform.position);
			sf::Vector2f screenPos = sf::Vector2f(pixelPos);

			if (!enemyBars.contains(id)) {
				createEnemyBar(id);
			}
			bool isHoveringTarget = hoveringTargetOpt.has_value() && (hoveringTargetOpt.value() == id);
			updateEnemyBar(id, stats.health, stats.getStat(STATS::MAX_HEALTH), screenPos, isHoveringTarget);
		}
	});

	for (auto it = enemyBars.begin(); it != enemyBars.end();) {
		if (std::find(activeEnemies.begin(), activeEnemies.end(), it->first) == activeEnemies.end()) {
			gui.remove(it->second);
			it = enemyBars.erase(it);
			spdlog::get("combat")->info("Remove bar for enemy");
		} else {
			++it;
		}
	}
}

void EnemyHealthBarSystem::createEnemyBar(EntityID id)
{
	auto bar = tgui::ProgressBar::create();
	bar->setSize(60, 10);
	bar->getRenderer()->setFillColor(sf::Color::Red);
	bar->getRenderer()->setBackgroundColor(sf::Color::White);
	bar->setText("");
	bar->getRenderer()->setBorders({1, 1, 1, 1});
	bar->getRenderer()->setBorderColor(sf::Color::Black);

	gui.add(bar);
	enemyBars[id] = bar;
}

void EnemyHealthBarSystem::updateEnemyBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos,
                                          bool isHoveringTarget)
{
	if (hp > maxHp) {
		spdlog::get("combat")->debug("In healthbar-system health {} is bigger than maxHp {} for entity {}", hp, maxHp,
		                             id.getId());
	}
	auto &bar = enemyBars[id];
	bar->setMaximum(maxHp);
	bar->setValue(hp);
	bar->setPosition(screenPos.x - (bar->getSize().x / 2.0f), screenPos.y - 40.0f);
	bar->setVisible(true);
	if (isHoveringTarget) {
		bar->getRenderer()->setBorders({2, 2, 2, 2});
		bar->getRenderer()->setBorderColor(sf::Color::Yellow);
	} else {
		bar->getRenderer()->setBorders({1, 1, 1, 1});
		bar->getRenderer()->setBorderColor(sf::Color::Black);
	}
}

void EnemyHealthBarSystem::clearAllBars()
{
	for (auto &[id, bar] : enemyBars) {
		gui.remove(bar);
	}
	enemyBars.clear();
}
