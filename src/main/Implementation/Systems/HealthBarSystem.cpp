#include "Abstract/Combat/Systems/HealthBarSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <spdlog/spdlog.h>

HealthBarSystem::HealthBarSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window)
    : System(manager), gui(gui), window(window)
{
}

void HealthBarSystem::update()
{
	bool battleIsActive = false;
	manager.view<BattleManagerComponent>().each([&](EntityID id, auto &bmc) { battleIsActive = true; });

	if (!battleIsActive) {
		clearAllBars();
		return;
	}
	std::optional<EntityID> hoveringTargetOpt = std::nullopt;
	manager.view<BattleComponent>().each([&](EntityID id, BattleComponent &battleComp) {
		if (battleComp.isActiveTurn && battleComp.controller == BATTLE_CONTROLLER::LOCAL_PLAYER) {
			hoveringTargetOpt = battleComp.hoveringTarget;
		}
	});
	auto playerIdOpt = WorldUtils::getPlayer(manager);
	if (!playerIdOpt.has_value()) {
		clearAllBars();
		return;
	}
	EntityID mainPlayerId = playerIdOpt.value();

	std::vector<EntityID> activeEntities;
	if (!manager.hasComponent<BattleComponent>(playerIdOpt.value())) {
		clearAllBars();
		return;
	}
	manager.view<BattleManagerComponent>().each([&](EntityID bmcId, BattleManagerComponent &bmc) {
		for (EntityID id : bmc.participants) {
			if (id == playerIdOpt.value()) {
				continue;
			}

			if (manager.hasComponent<DeathComponent>(id)) {
				continue;
			}

			activeEntities.push_back(id);

			auto &stats = manager.getComponent<StatsComponent>(id);
			auto &transform = manager.getComponent<TransformComponent>(id);

			sf::Vector2i pixelPos = window.mapCoordsToPixel(transform.position);
			sf::Vector2f screenPos = sf::Vector2f(pixelPos);

			if (!bars.contains(id)) {
				createBar(id, manager.getComponent<BattleComponent>(id).faction);
			}
			bool isHoveringTarget = hoveringTargetOpt.has_value() && (hoveringTargetOpt.value() == id);
			updateBar(id, stats.health, stats.getStat(STATS::MAX_HEALTH), screenPos, isHoveringTarget);
		}
	});

	for (auto it = bars.begin(); it != bars.end();) {
		if (std::find(activeEntities.begin(), activeEntities.end(), it->first) == activeEntities.end()) {
			gui.remove(it->second);
			it = bars.erase(it);
			spdlog::get("combat")->info("Remove bar");
		} else {
			++it;
		}
	}
}

void HealthBarSystem::createBar(EntityID id, BATTLE_FACTION faction)
{
	auto bar = tgui::ProgressBar::create();
	bar->setSize(60, 10);
	if (faction == BATTLE_FACTION::ENEMY) {
		bar->getRenderer()->setFillColor(sf::Color::Red);
	} else {
		bar->getRenderer()->setFillColor(sf::Color::Green);
	}

	bar->getRenderer()->setBackgroundColor(sf::Color::White);
	bar->setText("");
	bar->getRenderer()->setBorders({1, 1, 1, 1});
	bar->getRenderer()->setBorderColor(sf::Color::Black);

	gui.add(bar);
	bars[id] = bar;
}

void HealthBarSystem::updateBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos, bool isHoveringTarget)
{
	if (hp > maxHp) {
		spdlog::get("combat")->debug("In healthbar-system health {} is bigger than maxHp {} for entity {}", hp, maxHp,
		                             id.getId());
	}
	auto &bar = bars[id];
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

void HealthBarSystem::clearAllBars()
{
	for (auto &[id, bar] : bars) {
		gui.remove(bar);
	}
	bars.clear();
}
