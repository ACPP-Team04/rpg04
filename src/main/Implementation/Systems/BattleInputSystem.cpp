#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
BattleInputSystem::BattleInputSystem(ArchetypeManager &manager, tgui::Gui &gui) : System(manager), ui(gui)
{
	connectCallbacks();
}

void BattleInputSystem::connectCallbacks()
{
	ui.getButton("BtnLight")->onPress([this]() {
		auto player = manager.getEntityIdByTag(EntityTag::PLAYER)[0];
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::LIGHT_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnHeavy")->onPress([this]() {
		auto player = manager.getEntityIdByTag(EntityTag::PLAYER)[0];
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::HEAVY_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnUltimate")->onPress([this]() {
		auto player = manager.getEntityIdByTag(EntityTag::PLAYER)[0];
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::ULTIMATE_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnHeal")->onPress([this]() {
		auto player = manager.getEntityIdByTag(EntityTag::PLAYER)[0];
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::HEAL;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnRest")->onPress([this]() {
		auto player = manager.getEntityIdByTag(EntityTag::PLAYER)[0];
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::REST;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
}

void BattleInputSystem::update()
{
	auto players = manager.getEntityIdByTag(EntityTag::PLAYER);
	if (players.empty())
		return;
	EntityID playerId = players[0];

	// Fix me: add check for current world != battleWorld, then return
	auto &battle = manager.getComponent<BattleComponent>(playerId);
	auto &stats = manager.getComponent<StatsComponent>(playerId);
	auto &inv = manager.getComponent<InventoryComponent>(playerId);
	ui.setHUDVisible(true);
	bool showMenu = battle.battleState == BattleState::WAITING_FOR_INPUT;
	ui.setActionPanelVisible(showMenu);

	if (showMenu) {
		ui.updateStats(stats.health, stats.maxHealth, battle.AP);

		ui.getButton("BtnLight")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::LIGHT_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, inv.numberOfHealthPotions));
		ui.getButton("BtnHeavy")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::HEAVY_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, inv.numberOfHealthPotions));
		ui.getButton("BtnUltimate")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::ULTIMATE_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, inv.numberOfHealthPotions));
		ui.getButton("BtnHeal")->setEnabled(CombatSystem::validateAction(
		    BattleAction::HEAL, battle.AP, battle.numberOfUltimateAttacksUsed, inv.numberOfHealthPotions));
		ui.getButton("BtnRest")->setEnabled(true);
	}
	auto bmcId = manager.getComponent<BattleComponent>(playerId).battleManagerId;
	auto &bmc = manager.getComponent<BattleManagerComponent>(bmcId);

	// later logic for enemy HP bar
	/*
	for (EntityID id : bmc.participants) {
	    if (manager.getEntityTag(id) == EntityTag::ENEMY) {
	        if (!ui.hasEnemyBar(id))
	            ui.createEnemyBar(id);

	        auto &stats = manager.getComponent<StatsComponent>(id);
	        // auto &transform = manager.getComponent<TransformComponent>(id);
	        // sf::Vector2f screenPos = window.mapCoordsToPixel(transform.position);

	        // ui.updateEnemyBar(id, stats.health, stats.maxHealth, screenPos);
	    }
	}
	*/
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
