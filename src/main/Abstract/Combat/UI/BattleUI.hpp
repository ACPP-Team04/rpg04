#pragma once
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>

class BattleUI {
  private:
	tgui::Gui &gui;
	tgui::Panel::Ptr rootPanel;

  public:
	BattleUI(tgui::Gui &gui) : gui(gui) { setupLayout(); }

	void setupLayout()
	{
		rootPanel = tgui::Panel::create({"250", "280"});
		rootPanel->setPosition("5%", "60%");
		gui.add(rootPanel, "BattleMenu");

		auto layout = tgui::VerticalLayout::create();
		layout->setSize("90%", "85%");
		layout->setPosition("5%", "5%");
		rootPanel->add(layout);

		auto hpBar = tgui::ProgressBar::create();
		hpBar->getRenderer()->setFillColor(sf::Color::Red);
		hpBar->setText("HP");
		layout->add(hpBar, "HPBar");

		auto apLabel = tgui::Label::create("AP: 0");
		apLabel->getRenderer()->setTextColor(sf::Color::Blue);
		layout->add(apLabel, "APLabel");

		layout->add(tgui::Button::create("Light Attack"), "BtnLight");
		layout->add(tgui::Button::create("Heavy Attack"), "BtnHeavy");
		layout->add(tgui::Button::create("Ultimate"), "BtnUltimate");
		layout->add(tgui::Button::create("Heal"), "BtnHeal");
		layout->add(tgui::Button::create("Rest"), "BtnRest");

		rootPanel->setVisible(false);
	}

	tgui::Button::Ptr getButton(std::string name) { return rootPanel->get<tgui::Button>(name); }

	void setVisible(bool visible) { rootPanel->setVisible(visible); }

	void updateStats(float hp, float maxHp, int ap)
	{
		auto bar = rootPanel->get<tgui::ProgressBar>("HPBar");
		bar->setMaximum(maxHp);
		bar->setValue(hp);
		bar->setText(std::to_string((int)hp) + " / " + std::to_string((int)maxHp));

		rootPanel->get<tgui::Label>("APLabel")->setText("AP: " + std::to_string(ap));
	}
};
