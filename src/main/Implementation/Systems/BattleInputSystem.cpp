#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
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
			b.target =
			    selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
			b.battleState = BattleState::SELECTED_ACTION;
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
			b.target =
			    selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
			b.battleState = BattleState::SELECTED_ACTION;
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
			b.target =
			    selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
			b.battleState = BattleState::SELECTED_ACTION;
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
			b.target =
			    selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
			b.battleState = BattleState::SELECTED_ACTION;
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
			b.target =
			    selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
			b.battleState = BattleState::SELECTED_ACTION;
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
	int numberOfHealsUsed = battle.numberOfHealsUsed;
	ui.setHUDVisible(true);
	bool showMenu = battle.battleState == BattleState::WAITING_FOR_INPUT;
	ui.setActionPanelVisible(showMenu);

	if (showMenu) {
		ui.updateStats(stats.health, stats.getStat(MAX_HEALTH), battle.AP);

		ui.getButton("BtnLight")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::LIGHT_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealsUsed));
		ui.getButton("BtnHeavy")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::HEAVY_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealsUsed));
		ui.getButton("BtnUltimate")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::ULTIMATE_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealsUsed));
		ui.getButton("BtnHeal")->setEnabled(CombatSystem::validateAction(
		    BattleAction::HEAL, battle.AP, battle.numberOfUltimateAttacksUsed, numberOfHealsUsed));
		ui.getButton("BtnRest")->setEnabled(true);
	}
	auto bmcId = manager.getComponent<BattleComponent>(playerId).battleManagerId;
	auto &bmc = manager.getComponent<BattleManagerComponent>(bmcId);
}

EntityID BattleInputSystem::selectTarget(std::vector<EntityID> participants, EntityID playerId)
{
	for (EntityID entity : participants) {
		if (entity != playerId) {
			return entity;
		}
	}
	throw std::runtime_error("No valid target found");
}
