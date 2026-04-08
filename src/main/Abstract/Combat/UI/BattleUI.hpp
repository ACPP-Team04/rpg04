#pragma once
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>

class BattleUI {
  private:
	tgui::Gui &gui;
	tgui::Panel::Ptr hudPanel;
	tgui::Panel::Ptr actionPanel;
	std::unordered_map<EntityID, tgui::ProgressBar::Ptr> enemyBars;

  public:
	BattleUI(tgui::Gui &gui) : gui(gui) {};

	void setupLayout()
	{
		hudPanel = tgui::Panel::create({"250", "80"});

		gui.add(hudPanel, "BattleHUD");
		hudPanel->setPosition("5%", "55%");
		gui.add(hudPanel, "BattleHUD");
		auto hpBar = tgui::ProgressBar::create();
		hpBar->setSize("90%", "30px");
		hpBar->setPosition("5%", "10%");
		hudPanel->add(hpBar, "HPBar");

		auto apLabel = tgui::Label::create("AP: 0");
		apLabel->setPosition("5%", "60%");
		hudPanel->add(apLabel, "APLabel");

		actionPanel = tgui::Panel::create({"250", "200"});
		actionPanel->setPosition("5%", "68%");
		gui.add(actionPanel, "BattleMenu");

		auto layout = tgui::VerticalLayout::create();
		layout->setSize("90%", "90%");
		layout->setPosition("5%", "5%");
		actionPanel->add(layout);

		layout->add(tgui::Button::create("Light Attack"), "BtnLight");
		layout->add(tgui::Button::create("Heavy Attack"), "BtnHeavy");
		layout->add(tgui::Button::create("Ultimate"), "BtnUltimate");
		layout->add(tgui::Button::create("Heal"), "BtnHeal");
		layout->add(tgui::Button::create("Rest"), "BtnRest");

		hudPanel->setVisible(false);
		actionPanel->setVisible(false);
	}

	tgui::Button::Ptr getButton(std::string name) { return actionPanel->get<tgui::Button>(name); }

	void setActionPanelVisible(bool visible) { actionPanel->setVisible(visible); }
	void setHUDVisible(bool visible) { hudPanel->setVisible(visible); }
	void updateStats(float hp, float maxHp, int ap)
	{
		auto bar = hudPanel->get<tgui::ProgressBar>("HPBar");
		bar->setMaximum(maxHp);
		bar->setValue(hp);
		bar->setText(std::to_string((int)hp) + " / " + std::to_string((int)maxHp));

		hudPanel->get<tgui::Label>("APLabel")->setText("AP: " + std::to_string(ap));
	}
	void createEnemyBar(EntityID id)
	{
		auto bar = tgui::ProgressBar::create();
		bar->setSize(60, 10);
		bar->getRenderer()->setFillColor(sf::Color::Red);
		bar->getRenderer()->setBackgroundColor(sf::Color(50, 50, 50));
		bar->setText("");

		gui.add(bar);
		enemyBars[id] = bar;
	}

	void updateEnemyBar(EntityID id, float hp, float maxHp, sf::Vector2f screenPos)
	{
		if (enemyBars.contains(id)) {
			auto &bar = enemyBars[id];
			bar->setMaximum(maxHp);
			bar->setValue(hp);
			bar->setPosition(screenPos.x - (bar->getSize().x / 2.0f), screenPos.y - 40.0f);
			bar->setVisible(true);
		}
	}

	void removeEnemyBar(EntityID id)
	{
		if (enemyBars.contains(id)) {
			gui.remove(enemyBars[id]);
			enemyBars.erase(id);
		}
	}

	bool hasEnemyBar(EntityID id) { return enemyBars.contains(id); }
};
