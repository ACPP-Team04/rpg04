
#include "Abstract/ECS/System/System.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
struct MenuSystem : System {

	MenuSystem(ArchetypeManager& manager, tgui::Gui &gui);

	void update() override;
	tgui::Gui& gui;
};