#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
BattleInputSystem::BattleInputSystem(ArchetypeManager &manager, tgui::Gui &gui)
    : System(manager), ui(gui) {

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
	}

	ui.getButton("BtnLight")->onPress([&]() {
		auto &b = manager.getComponent<BattleComponent>(playerId);
		b.selectedAction = BattleAction::LIGHT_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnHeavy")->onPress([&]() {
		auto &b = manager.getComponent<BattleComponent>(playerId);
		b.selectedAction = BattleAction::HEAVY_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, playerId);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnUltimate")->onPress([&]() {
		auto player = playerId;
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::ULTIMATE_ATTACK;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnHeal")->onPress([&]() {
		auto player = playerId;
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::HEAL;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
	ui.getButton("BtnRest")->onPress([&]() {
		auto player = playerId;
		auto &b = manager.getComponent<BattleComponent>(player);
		b.selectedAction = BattleAction::REST;
		b.target = selectTarget(manager.getComponent<BattleManagerComponent>(b.battleManagerId).participants, player);
		b.battleState = BattleState::SELECTED_ACTION;
	});
}

void BattleInputSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();

	if (view.archetypes.size() == 0) {
		return;
	}
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	}

	auto &battle = manager.getComponent<BattleComponent>(playerId);
	auto &stats = manager.getComponent<StatsComponent>(playerId);
	auto &inv = manager.getComponent<InventoryComponent>(playerId);
	std::vector<EntityID> healPositions;
	for (auto &item : inv.inventory) {
		bool IsHealItem = this->manager.hasComponent<ITEM_HEALSTATS_COMPONENT>(item);
		if (IsHealItem) {
			healPositions.push_back(item);
		}
	}
	int numberOfHealPotions = healPositions.size();
	ui.setHUDVisible(true);
	bool showMenu = battle.battleState == BattleState::WAITING_FOR_INPUT;
	ui.setActionPanelVisible(showMenu);

	if (showMenu) {
		ui.updateStats(stats.health, stats.maxHealth, battle.AP);

		ui.getButton("BtnLight")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::LIGHT_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealPotions));
		ui.getButton("BtnHeavy")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::HEAVY_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealPotions));
		ui.getButton("BtnUltimate")
		    ->setEnabled(CombatSystem::validateAction(BattleAction::ULTIMATE_ATTACK, battle.AP,
		                                              battle.numberOfUltimateAttacksUsed, numberOfHealPotions));
		ui.getButton("BtnHeal")->setEnabled(CombatSystem::validateAction(
		    BattleAction::HEAL, battle.AP, battle.numberOfUltimateAttacksUsed, numberOfHealPotions));
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
