#pragma once
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <format>
#include <string>

class BattleUI {
  private:
	tgui::Gui &gui;
	tgui::Panel::Ptr hudPanel;
	tgui::Panel::Ptr actionPanel;

  public:
	explicit BattleUI(tgui::Gui &gui) : gui(gui) {};

	void setupLayout()
	{
		hudPanel = tgui::Panel::create({"250", "80"});

		gui.add(hudPanel, "BattleHUD");
		hudPanel->setPosition("-1000", "-1000");
		auto hpBar = tgui::ProgressBar::create();
		hpBar->setSize("90%", "30px");
		hpBar->setPosition("5%", "10%");
		hudPanel->add(hpBar, "HPBar");

		auto apLabel = tgui::Label::create("AP: 0");
		apLabel->setPosition("5%", "60%");
		hudPanel->add(apLabel, "APLabel");

		actionPanel = tgui::Panel::create({"250", "230"});
		actionPanel->setPosition("-1000", "-1000");
		gui.add(actionPanel, "BattleMenu");

		auto layout = tgui::VerticalLayout::create();
		layout->setSize("90%", "90%");
		layout->setPosition("5%", "5%");
		actionPanel->add(layout);

		auto turnLabel = tgui::Label::create("Hero's Turn");
		turnLabel->setTextSize(16);
		turnLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
		layout->add(turnLabel, "TurnLabel");

		layout->add(tgui::Button::create("Light Attack"), "BtnLight");
		layout->add(tgui::Button::create("Heavy Attack"), "BtnHeavy");
		layout->add(tgui::Button::create("Ultimate"), "BtnUltimate");
		layout->add(tgui::Button::create("Heal"), "BtnHeal");
		layout->add(tgui::Button::create("Rest"), "BtnRest");

		auto confirmHint = tgui::Label::create("Press <<Enter>> to confirm attack");
		confirmHint->setTextSize(11);
		confirmHint->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
		confirmHint->setVerticalAlignment(tgui::VerticalAlignment::Center);
		confirmHint->getRenderer()->setTextColor(tgui::Color(70, 70, 70));
		layout->add(confirmHint, "ConfirmHint");

		hudPanel->setVisible(false);
		actionPanel->setVisible(false);
	}

	tgui::Button::Ptr getButton(const std::string &name) const { return actionPanel->get<tgui::Button>(name); }

	tgui::Label::Ptr getLabel(const std::string &name) const { return actionPanel->get<tgui::Label>(name); }

	void setActionPanelVisible(bool visible) const { actionPanel->setVisible(visible); }
	void setHUDVisible(bool visible) const { hudPanel->setVisible(visible); }
	void updateStats(float hp, float maxHp, float ap) const
	{
		auto bar = hudPanel->get<tgui::ProgressBar>("HPBar");
		bar->setMaximum(static_cast<unsigned int>(maxHp));
		bar->setValue(static_cast<unsigned int>(hp));
		bar->setText(std::format("{} / {}", (int)hp, (int)maxHp));
		hudPanel->get<tgui::Label>("APLabel")->setText(std::format("AP: {}", static_cast<unsigned int>(ap)));
	}
	void updateDynamicPosition(float playerX, float screenMiddleX) const
	{
		if (playerX > screenMiddleX) {
			hudPanel->setPosition("5%", "100% - 330");
			actionPanel->setPosition("5%", "100% - 240");
		} else {
			hudPanel->setPosition("95% - 250", "100% - 330");
			actionPanel->setPosition("95% - 250", "100% - 240");
		}
	}
};
