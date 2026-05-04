#pragma once
#include "Abstract/ECS/System/System.hpp"
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <unordered_map>

class EnemyHealthBarSystem : public System {
  public:
	EnemyHealthBarSystem(ArchetypeManager &manager, tgui::Gui &gui, sf::RenderWindow &window);
	void update() override;

  private:
	tgui::Gui &gui;
	sf::RenderWindow &window;
	std::unordered_map<EntityID, tgui::ProgressBar::Ptr> enemyBars;
	void createEnemyBar(EntityID id);
	void updateEnemyBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos, bool isHoveringTarget);
	void clearAllBars();
};
