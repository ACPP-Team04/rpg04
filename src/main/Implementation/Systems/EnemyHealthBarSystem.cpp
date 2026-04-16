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
	auto view = manager.view<BattleManagerComponent>();

	if (view.archetypes.size() == 0) {
		clearAllBars();
		return;
	}

	std::vector<EntityID> activeEnemies;
	auto playerId = WorldUtils::getPlayer(manager);
	if (!playerId.has_value()) {
		throw std::runtime_error("Player entity not found in EnemyHealthBarSystem");
	}
	view.each([&](EntityID bmcId, BattleManagerComponent &bmc) {
		for (EntityID id : bmc.participants) {
			if (id == playerId.value()) {
				continue;
			}

			activeEnemies.push_back(id);

			auto &stats = manager.getComponent<StatsComponent>(id);
			auto &transform = manager.getComponent<TransformComponent>(id);

			// Handle Coordinate Projection
			sf::Vector2i pixelPos = window.mapCoordsToPixel(transform.position);
			sf::Vector2f screenPos = sf::Vector2f(pixelPos);

			if (!enemyBars.contains(id)) {
				createEnemyBar(id);
			}
			updateEnemyBar(id, stats.health, stats.getStat(STATS::MAX_HEALTH), screenPos);
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

	gui.add(bar);
	enemyBars[id] = bar;
}

void EnemyHealthBarSystem::updateEnemyBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos)
{
	if (hp > maxHp) {
		spdlog::get("combat")->info("In healthbar-system health {} is bigger than maxHp {} for entity {}", hp, maxHp,
		                            id.getId());
	}
	auto &bar = enemyBars[id];
	bar->setMaximum(maxHp);
	bar->setValue(hp);
	bar->setPosition(screenPos.x - (bar->getSize().x / 2.0f), screenPos.y - 40.0f);
	bar->setVisible(true);
}

void EnemyHealthBarSystem::clearAllBars()
{
	for (auto &[id, bar] : enemyBars) {
		gui.remove(bar);
	}
	enemyBars.clear();
}
