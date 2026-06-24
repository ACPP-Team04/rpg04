#include "Abstract/UI/GameOverMenu.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include <Abstract/Persistance/SaveManager.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>

void GameOverMenu::setUpGameOverMenu(tgui::Gui &gui, GameState &state)
{
	auto panel = tgui::Panel::create({"100%", "100%"});
	panel->getRenderer()->setBackgroundColor(tgui::Color(20, 0, 0, 230));
	gui.add(panel, "GameOverMenu");

	auto title = tgui::Label::create("YOU DIED");
	title->setTextSize(72);
	title->setPosition("50% - width / 2", "30%");
	title->getRenderer()->setTextColor(tgui::Color::Red);
	panel->add(title);

	auto loadButton = tgui::Button::create("Load Last Save");
	loadButton->setSize(250, 60);
	loadButton->setPosition("50% - width / 2", "50%");
	if (SaveManager::doesSaveExist(1)) {
		loadButton->onPress([&gui, &state] {
			PersistenceManager::getInstance().requestLoad = true;
			state = GameState::Game;
			gui.remove(gui.get("GameOverMenu"));
		});
	} else {
		loadButton->setEnabled(false);
		loadButton->setText("No Save Found");
	}
	panel->add(loadButton);

	auto quitButton = tgui::Button::create("Quit to Desktop");
	quitButton->setSize(250, 60);
	quitButton->setPosition("50% - width / 2", "60%");
	quitButton->onPress([&state] {
		PersistenceManager::getInstance().requestQuit = true;
		state = GameState::Quit;
	});
	panel->add(quitButton);
}