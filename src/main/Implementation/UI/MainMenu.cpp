#include "Abstract/UI/MainMenu.hpp"

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Label.hpp>

namespace {
constexpr auto MainMenuWidgetName = "MainMenu";

void hideMainMenu(tgui::Gui &gui)
{
	if (auto mainMenu = gui.get(MainMenuWidgetName)) {
		mainMenu->setVisible(false);
	}
}
}

void setLayout(tgui::Gui &gui, GameState &state)
{
	auto mainMenu = tgui::Group::create({"100%", "100%"});
	gui.add(mainMenu, MainMenuWidgetName);

	auto title = tgui::Label::create("Zombie Knight");
	title->setTextSize(48);
	title->setPosition("50% - width / 2", "20%");
	mainMenu->add(title);

	auto startButton = tgui::Button::create("Start Game");
	startButton->setSize(220, 50);
	startButton->setPosition("50% - width / 2", "45%");
	startButton->onPress([&gui, &state] {
		hideMainMenu(gui);
		state = GameState::Game;
	});
	mainMenu->add(startButton);

	auto quitButton = tgui::Button::create("Quit");
	quitButton->setSize(220, 50);
	quitButton->setPosition("50% - width / 2", "55%");
	quitButton->onPress([&] { state = GameState::Quit; });
	mainMenu->add(quitButton);
}

void setUpMainMenu(tgui::Gui &gui, GameState &state)
{
	setLayout(gui, state);
}
