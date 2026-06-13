#pragma once
#include "Abstract/UI/MainMenu.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>

class GameOverMenu {
  public:
	static void setUpGameOverMenu(tgui::Gui &gui, GameState &state);
};