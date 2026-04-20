#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
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
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	} else {
		return;
	}

	ui.getButton("BtnLight")->onPress([this]() {
		auto player = WorldUtils::getPlayer(manager);
		EntityID playerId;
		if (player.has_value()) {
			playerId = player.value();
		}
		if (manager.hasComponent<BattleComponent>(playerId)) {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.selectedAction = BattleAction::LIGHT_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnHeavy")->onPress([this]() {
		auto player = WorldUtils::getPlayer(manager);
		EntityID playerId;
		if (player.has_value()) {
			playerId = player.value();
		}
		if (manager.hasComponent<BattleComponent>(playerId)) {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.selectedAction = BattleAction::HEAVY_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnUltimate")->onPress([this]() {
		auto player = WorldUtils::getPlayer(manager);
		EntityID playerId;
		if (player.has_value()) {
			playerId = player.value();
		}
		if (manager.hasComponent<BattleComponent>(playerId)) {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.selectedAction = BattleAction::ULTIMATE_ATTACK;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnHeal")->onPress([this]() {
		auto player = WorldUtils::getPlayer(manager);
		EntityID playerId;
		if (player.has_value()) {
			playerId = player.value();
		}
		if (manager.hasComponent<BattleComponent>(playerId)) {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.selectedAction = BattleAction::HEAL;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
	ui.getButton("BtnRest")->onPress([this]() {
		auto player = WorldUtils::getPlayer(manager);
		EntityID playerId;
		if (player.has_value()) {
			playerId = player.value();
		}
		if (manager.hasComponent<BattleComponent>(playerId)) {
			auto &b = manager.getComponent<BattleComponent>(playerId);
			b.selectedAction = BattleAction::REST;
			b.battleState = BattleState::SELECTING_TARGET;
		}
	});
}

void BattleInputSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();

	if (view.archetypes.size() == 0) {
		return;
	}
	auto player = WorldUtils::getPlayer(manager);
	if (!player.has_value()) {
		return;
	}
	EntityID playerId = player.value();

	if (!manager.hasComponent<BattleComponent>(playerId)) {
		ui.setHUDVisible(false);
		return;
	}
	auto &battle = manager.getComponent<BattleComponent>(playerId);
	auto &stats = manager.getComponent<StatsComponent>(playerId);

	ui.setHUDVisible(true);
	bool showMenu = battle.battleState == BattleState::WAITING_FOR_INPUT;
	ui.setActionPanelVisible(showMenu);

	bool showTargetMenu = battle.battleState == BattleState::SELECTING_TARGET;
	if (showMenu) {
		ui.updateStats(stats.health, stats.getStat(MAX_HEALTH), battle.AP);

		ui.getButton("BtnLight")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::LIGHT_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed));
		ui.getButton("BtnHeavy")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::HEAVY_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed));
		ui.getButton("BtnUltimate")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::ULTIMATE_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed));
		ui.getButton("BtnHeal")->setEnabled(
		    CombatSystem::validateAction(BattleAction::HEAL, battle.AP, battle.numberOfUltimateAttacksUsed));
		ui.getButton("BtnRest")->setEnabled(true);
	} else if (showTargetMenu) {
		auto validTargets =
		    getTargetsInBattle(playerId, manager.getComponent<BattleComponent>(playerId).battleManagerId);
		if (validTargets.empty()) {
			throw std::runtime_error("No valid targets in battle for player");
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
	}
}

std::vector<EntityID> BattleInputSystem::getTargetsInBattle(const EntityID playerId, const EntityID battleMangerId)
{
	const auto &bmc = manager.getComponent<BattleManagerComponent>(battleMangerId).participants;
	std::vector<EntityID> targets;
	for (EntityID entity : bmc) {
		// TODO: add logic for not attacking companions later
		if (entity != playerId && !manager.hasComponent<DeathComponent>(entity)) {
			targets.push_back(entity);
		}
	}
	return targets;
}
