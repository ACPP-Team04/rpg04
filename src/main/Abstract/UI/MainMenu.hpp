#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

enum class GameState {
	MainMenu,
	Game,
	Quit
};

void setLayout(tgui::Gui &gui, GameState &state);
void setUpMainMenu(tgui::Gui &gui, GameState &state);
