#include "Abstract/UI/MainMenu.hpp"

#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include <Abstract/Persistance/SaveManager.hpp>
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
} // namespace

void setLayout(tgui::Gui &gui, GameState &state)
{
	auto mainMenu = tgui::Group::create({"100%", "100%"});
	gui.add(mainMenu, MainMenuWidgetName);

	auto title = tgui::Label::create("Zombie Knight");
	title->setTextSize(48);
	title->setPosition("50% - width / 2", "20%");
	mainMenu->add(title);

	auto startButton = tgui::Button::create("Start New Game");
	startButton->setSize(220, 50);
	startButton->setPosition("50% - width / 2", "40%");
	startButton->onPress([&gui, &state] {
		hideMainMenu(gui);
		state = GameState::Game;
	});
	mainMenu->add(startButton);

	auto loadButton = tgui::Button::create("Load Game");
	loadButton->setSize(220, 50);
	loadButton->setPosition("50% - width / 2", "50%");
	if (!SaveManager::doesSaveExist(1)) {
		loadButton->setText("No Save Found");
		loadButton->setEnabled(false);
	}
	loadButton->onPress([&gui, &state, loadButton] {
		if (SaveManager::doesSaveExist(1)) {
			hideMainMenu(gui);
			state = GameState::Game;
			PersistenceManager::getInstance().requestLoad = true;
		} else {
			spdlog::warn("Player tried to load, but no save file exists!");
			loadButton->setText("No Save Found!");
		}
	});
	mainMenu->add(loadButton);

	auto quitButton = tgui::Button::create("Quit");
	quitButton->setSize(220, 50);
	quitButton->setPosition("50% - width / 2", "60%");
	quitButton->onPress([&] { state = GameState::Quit; });
	mainMenu->add(quitButton);
}

void setUpMainMenu(tgui::Gui &gui, GameState &state)
{
	setLayout(gui, state);
}
