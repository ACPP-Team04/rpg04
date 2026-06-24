#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
BattleInputSystem::BattleInputSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window)
    : System(manager), ui(gui), window(window) {

      };

void BattleInputSystem::init()
{
	ui.setupLayout();
	connectCallbacks();
}

void BattleInputSystem::connectCallbacks()
{
	ui.getButton("BtnLight")->onPress([this]() {
		auto activeIdOpt = getActiveLocalController();
		if (!activeIdOpt.has_value()) {
			return;
		}
		if (manager.hasComponent<BattleComponent>(activeIdOpt.value())) {
			auto &b = manager.getComponent<BattleComponent>(activeIdOpt.value());
			b.selectedAction = BattleAction::LIGHT_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnHeavy")->onPress([this]() {
		auto activeIdOpt = getActiveLocalController();
		if (!activeIdOpt.has_value()) {
			return;
		}
		if (manager.hasComponent<BattleComponent>(activeIdOpt.value())) {
			auto &b = manager.getComponent<BattleComponent>(activeIdOpt.value());
			b.selectedAction = BattleAction::HEAVY_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnUltimate")->onPress([this]() {
		auto activeIdOpt = getActiveLocalController();
		if (!activeIdOpt.has_value()) {
			return;
		}
		if (manager.hasComponent<BattleComponent>(activeIdOpt.value())) {
			auto &b = manager.getComponent<BattleComponent>(activeIdOpt.value());
			b.selectedAction = BattleAction::ULTIMATE_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnHeal")->onPress([this]() {
		auto activeIdOpt = getActiveLocalController();
		if (!activeIdOpt.has_value()) {
			return;
		}
		if (manager.hasComponent<BattleComponent>(activeIdOpt.value())) {
			auto &b = manager.getComponent<BattleComponent>(activeIdOpt.value());
			b.selectedAction = BattleAction::HEAL;
			b.battleState = BattleState::SELECTING_TARGET;
			b.target = activeIdOpt.value();
		}
	});
	ui.getButton("BtnRest")->onPress([this]() {
		auto activeIdOpt = getActiveLocalController();
		if (!activeIdOpt.has_value()) {
			return;
		}
		if (manager.hasComponent<BattleComponent>(activeIdOpt.value())) {
			auto &b = manager.getComponent<BattleComponent>(activeIdOpt.value());
			b.selectedAction = BattleAction::REST;
			b.battleState = BattleState::SELECTING_TARGET;
			b.target = activeIdOpt.value();
		}
	});
}

std::optional<EntityID> BattleInputSystem::getActiveLocalController()
{
	std::optional<EntityID> activeEntity = std::nullopt;

	manager.view<BattleManagerComponent>().each([&](EntityID bmcId, BattleManagerComponent &bmc) {
		if (bmc.participants.empty() || bmc.isBattleOver) {
			spdlog::debug(
			    "BattleManagerComponent has no participants or battle is over. Cannot find active local controller.");
			return;
		}

		for (EntityID participant : bmc.participants) {
			if (manager.hasComponent<BattleComponent>(participant)) {
				auto &bComp = manager.getComponent<BattleComponent>(participant);
				if (bComp.isActiveTurn && bComp.controller == BATTLE_CONTROLLER::LOCAL_PLAYER) {
					activeEntity = participant;
				}
			}
		}
	});
	return activeEntity;
}

void BattleInputSystem::update()
{
	bool battleIsActive = false;
	manager.view<BattleManagerComponent>().each([&](EntityID id, auto &bmc) { battleIsActive = true; });
	if (!battleIsActive) {
		ui.setHUDVisible(false);
		ui.setActionPanelVisible(false);
		return;
	}

	auto activeIdOpt = getActiveLocalController();
	if (!activeIdOpt.has_value()) {
		return;
	}

	EntityID activeId = activeIdOpt.value();
	if (manager.getComponent<BattleComponent>(activeId).battleState == BattleState::STATS_DISTRIBUTION) {
		ui.setHUDVisible(false);
		ui.setActionPanelVisible(false);
		return;
	}
	auto &battle = manager.getComponent<BattleComponent>(activeId);
	auto &stats = manager.getComponent<CharacterComponent>(activeId).stats;
	bool showMenu = battle.battleState == BattleState::WAITING_FOR_INPUT;

	bool showTargetMenu = battle.battleState == BattleState::SELECTING_TARGET;
	if (showMenu) {
		auto &activeIdTransform = manager.getComponent<TransformComponent>(activeId);
		float screenMiddleX = WORLD_SIZE_X / 2.0f;

		ui.updateDynamicPosition(activeIdTransform.position.x, screenMiddleX);
		ui.setHUDVisible(true);
		ui.setActionPanelVisible(showMenu);
		auto btnLightAttack = ui.getButton("BtnLight");
		btnLightAttack->setEnabled(CombatSystem::validateAction(BattleAction::LIGHT_ATTACK, battle));
		auto btnHeavy = ui.getButton("BtnHeavy");
		btnHeavy->setEnabled(CombatSystem::validateAction(BattleAction::HEAVY_ATTACK, battle));
		auto btnUltimate = ui.getButton("BtnUltimate");
		btnUltimate->setEnabled(CombatSystem::validateAction(BattleAction::ULTIMATE_ATTACK, battle));
		auto btnHeal = ui.getButton("BtnHeal");
		btnHeal->setEnabled(CombatSystem::validateAction(BattleAction::HEAL, battle));
		auto btnRest = ui.getButton("BtnRest");
		btnRest->setEnabled(CombatSystem::validateAction(BattleAction::REST, battle));

		if (battle.AP != lastDrawnAP || activeId != lastActiveId) {
			auto turnLabel = ui.getLabel("TurnLabel");
			auto playerOpt = WorldUtils::getPlayer(manager);

			if (playerOpt.has_value() && activeId == playerOpt.value()) {
				turnLabel->setText("Hero's Turn");
			} else {
				turnLabel->setText("Companion's Turn");
			}
			ui.updateStats(stats.health, stats.getStat(MAX_HEALTH), battle.AP);

			btnLightAttack->setText("Light Attack (-"
			                        + std::to_string(CombatSystem::getActionCost(BattleAction::LIGHT_ATTACK)) + " AP)");

			btnHeavy->setText("Heavy Attack (-"
			                  + std::to_string(CombatSystem::getActionCost(BattleAction::HEAVY_ATTACK)) + " AP)");

			int ultimateCost = CombatSystem::getActionCost(BattleAction::ULTIMATE_ATTACK);
			int ultimateAttacksLeft = battle.maxUltimateAttacks - battle.numberOfUltimateAttacksUsed;
			btnUltimate->setText("Ultimate Attack (" + std::to_string(ultimateCost) + " AP) ["
			                     + std::to_string(ultimateAttacksLeft) + " Left]");

			int healCost = CombatSystem::getActionCost(BattleAction::HEAL);
			int healsLeft = battle.maxHeals - battle.numberOfHealsUsed;
			btnHeal->setText("Heal (-" + std::to_string(healCost) + " AP) [" + std::to_string(healsLeft) + " Left]");

			btnRest->setText("Rest (+2 AP)");

			lastDrawnAP = battle.AP;
			lastActiveId = activeId;
		}

	} else if (showTargetMenu) {
		auto validTargets = getTargetsInBattle(
		    activeId, manager.getComponent<BattleComponent>(activeId).battleManagerId, this->manager);
		if (validTargets.empty()) {
			throw std::runtime_error("No valid targets in battle for player");
		}
		auto selectedAction = battle.selectedAction;
		if (selectedAction == BattleAction::HEAL || selectedAction == BattleAction::REST) {
			battle.battleState = BattleState::SELECTED_ACTION;
			battle.hoveringTarget = std::nullopt;
			ui.setActionPanelVisible(false);
			return;
		}
		std::sort(validTargets.begin(), validTargets.end(), [this](const EntityID a, const EntityID b) {
			auto transformA = manager.getComponent<TransformComponent>(a);
			auto transformB = manager.getComponent<TransformComponent>(b);
			return transformA.position.x < transformB.position.x;
		});
		if (currentTargetIndex >= validTargets.size()) {
			currentTargetIndex = 0;
		}
		if (!battle.hoveringTarget.has_value()) {
			battle.hoveringTarget = validTargets[currentTargetIndex];
		}
		bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
		if (rightPressed && !rightKeyWasPressed) {
			currentTargetIndex = (currentTargetIndex + 1) % validTargets.size();
			battle.hoveringTarget = validTargets[currentTargetIndex];
		}
		rightKeyWasPressed = rightPressed;

		bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
		if (leftPressed && !leftKeyWasPressed) {
			currentTargetIndex = (currentTargetIndex == 0) ? (validTargets.size() - 1) : (currentTargetIndex - 1);
			battle.hoveringTarget = validTargets[currentTargetIndex];
		}
		leftKeyWasPressed = leftPressed;

		bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
		if (enterPressed && !enterKeyWasPressed) {
			battle.target = validTargets[currentTargetIndex];
			battle.battleState = BattleState::SELECTED_ACTION;
			battle.hoveringTarget = std::nullopt;
			ui.setActionPanelVisible(false);
		}
		enterKeyWasPressed = enterPressed;
	} else {
		lastDrawnAP = -1;
	}
}

std::vector<EntityID> BattleInputSystem::getTargetsInBattle(const EntityID playerId, const EntityID battleMangerId,
                                                            ArchetypeManager &manager)
{
	const auto &bmcParticipants = manager.getComponent<BattleManagerComponent>(battleMangerId).participants;
	const auto &battleComp = manager.getComponent<BattleComponent>(playerId);
	std::vector<EntityID> validTargets;
	for (EntityID p : bmcParticipants) {
		if (manager.hasComponent<DeathComponent>(p)) {
			continue;
		}

		if (manager.hasComponent<BattleComponent>(p)) {
			auto &targetBattleComp = manager.getComponent<BattleComponent>(p);

			if (targetBattleComp.faction != battleComp.faction) {
				validTargets.push_back(p);
			}
		}
	}
	return validTargets;
}
