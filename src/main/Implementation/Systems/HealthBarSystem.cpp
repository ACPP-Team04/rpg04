#include "Abstract/Combat/Systems/HealthBarSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <spdlog/spdlog.h>

HealthBarSystem::HealthBarSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window)
    : System(manager), gui(gui), window(window)
{
}

void HealthBarSystem::update()
{
	auto playerIdOpt = WorldUtils::getPlayer(manager);
	if (!playerIdOpt.has_value() || !manager.hasComponent<BattleComponent>(playerIdOpt.value())) {
		return;
	}
	EntityID mainPlayerId = playerIdOpt.value();

	auto bmcView = manager.view<BattleManagerComponent>();
	if (bmcView.archetypes.empty()) {
		return;
	}

	std::optional<EntityID> hoveringTargetOpt = std::nullopt;
	manager.view<BattleComponent>().each([&hoveringTargetOpt](EntityID id, BattleComponent &battleComp) {
		if (battleComp.isActiveTurn && battleComp.controller == BATTLE_CONTROLLER::LOCAL_PLAYER) {
			hoveringTargetOpt = battleComp.hoveringTarget;
		}
	});

	bmcView.each([&mainPlayerId, &hoveringTargetOpt, this](EntityID bmcId, const BattleManagerComponent &bmc) {
		for (EntityID id : bmc.participants) {
			if (id == mainPlayerId || manager.hasComponent<DeathComponent>(id)) {
				continue;
			}

			const auto &stats = manager.getComponent<CharacterComponent>(id).stats;
			const auto &transform = manager.getComponent<TransformComponent>(id);
			const auto &battle = manager.getComponent<BattleComponent>(id);

			auto maxHp = static_cast<float>(stats.getStat(STATS::MAX_HEALTH));
			float hpPercent = std::clamp(static_cast<float>(stats.health) / maxHp, 0.0f, 1.0f);

			float barWidth = 40.f;
			float barHeight = 5.f;

			sf::Vector2f barPos = transform.position + sf::Vector2f(-barWidth / 2.f, -25.f);

			bool isHoveringTarget = hoveringTargetOpt.has_value() && (hoveringTargetOpt.value() == id);

			sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
			bg.setPosition(barPos);
			bg.setFillColor(sf::Color::White);

			if (isHoveringTarget) {
				bg.setOutlineThickness(2.f);
				bg.setOutlineColor(sf::Color::Yellow);
			} else {
				bg.setOutlineThickness(1.f);
				bg.setOutlineColor(sf::Color::Black);
			}
			sf::Color fgColor;
			if (battle.faction == BATTLE_FACTION::PLAYER_PARTY) {
				fgColor = sf::Color::Green;
			} else {
				fgColor = sf::Color::Red;
			}

			sf::RectangleShape fg(sf::Vector2f(barWidth * hpPercent, barHeight));
			fg.setPosition(barPos);
			fg.setFillColor(fgColor);

			window.draw(bg);
			window.draw(fg);
		}
	});
}