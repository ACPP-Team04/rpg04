#include "Abstract/Overwordl/HudSystem.hpp"

#include "../../../../vcpkg/buildtrees/tgui/src/v1.8.0-08e9099a90.clean/include/TGUI/Widgets/ChatBox.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/PanelListBox.hpp>
HudSystem::HudSystem(ArchetypeManager &manager, sf::RenderWindow &window, tgui::Gui &gui)
    : System(manager), window(window), gui(gui)
{
}

bool isHudPanelOpened(tgui::Gui &gui, std::string name)
{
	auto panel = gui.get<tgui::Panel>(name);
	return panel != nullptr;
}

void openHudPanel(tgui::Gui &gui)
{
	gui.loadWidgetsFromFile(HUD_WIGDETS_PATH, false);
}

void closePanel(tgui::Gui &gui, std::string name)
{
	if (isHudPanelOpened(gui, name)) {
		gui.remove(gui.get<tgui::Panel>(name));
	}
}

void HudSystem::update()
{
	elpasedFramesSinceLastMessage++;
	WorldComponent *component = WorldUtils::getWorld(manager);

	if (!isHudPanelOpened(gui, "HudPanel")) {
		openHudPanel(gui);
	}

	std::shared_ptr<tgui::ChatBox> toastBox = gui.get<tgui::ChatBox>("ToastChatBox");
	std::shared_ptr<tgui::ChatBox> persistantMessageBox = gui.get<tgui::ChatBox>("PersistanMessageBox");

	if (elpasedFramesSinceLastMessage > TOAST_FRAME_DURATION) {
		size_t size = toastBox.get()->getLineAmount();
		if (size > 0) {
			toastBox->removeLine(size - 1);
		}
		elpasedFramesSinceLastMessage = 0;
	}
	if (component->hasMessageInHud()) {
		toastBox->addLine(component->readMessageFromHud());
	}

	persistantMessageBox.get()->removeAllLines();
	for (auto message : component->persistentMessages) {
		persistantMessageBox.get()->addLine(message);
	}
}