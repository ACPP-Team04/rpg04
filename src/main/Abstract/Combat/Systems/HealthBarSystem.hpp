#pragma once
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <unordered_map>

class HealthBarSystem : public System {
  public:
	HealthBarSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window);
	void update() override;

  private:
	tgui::Gui &gui;
	sf::RenderWindow &window;
	std::unordered_map<EntityID, tgui::ProgressBar::Ptr> bars;
	void createBar(EntityID id, BATTLE_FACTION faction);
	void updateBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos, bool isHoveringTarget);
	void clearAllBars();
};
