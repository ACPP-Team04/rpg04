#include "Abstract/Overwordl/DialogSystem.hpp"

#include "../../../../vcpkg/buildtrees/tgui/src/v1.8.0-08e9099a90.clean/include/TGUI/Widgets/Picture.hpp"
#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Overwordl/CollisionSystem.hpp"
#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsInInventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/PanelListBox.hpp>

DialogSystem::DialogSystem(ArchetypeManager &manager, sf::RenderWindow &window, tgui::Gui &gui)
    : System(manager), window(window), gui(gui)
{
}

bool isPanelOpened(tgui::Gui &gui, std::string name)
{
	auto panel = gui.get<tgui::Panel>(name);
	return panel != nullptr;
}

void openPanel(tgui::Gui &gui)
{
	gui.loadWidgetsFromFile(DIALOG_WIDGETS_PATH, false);
}

void openPanelIfNotOpened(tgui::Gui &gui)
{
	if (isPanelOpened(gui, "DialogPanel")) {
		return;
	}
	openPanel(gui);
}

template <typename T>
std::shared_ptr<T> getWidget(tgui::Gui &gui, std::string widgetName)
{
	auto widget = gui.get<T>(widgetName);

	if (!widget) {
		throw tgui::Exception("getWidget(): Widget " + widgetName + " not found");
	}
	return widget;
}

void deletePanel(tgui::Gui &gui, std::string name)
{
	auto panel = gui.get<tgui::Panel>(name);
	if (!panel) {
		return;
	}
	gui.remove(panel);
}

void closeDialogPanelIfOpened(tgui::Gui &gui)
{
	deletePanel(gui, "DialogPanel");
}

void setPicture(ArchetypeManager &manager, int speakerId, std::shared_ptr<tgui::Picture> speakerPicture)
{
	if (!manager.hasComponent<SpriteComponent>(EntityID::fromExistingId(speakerId))) {
		return;
	}

	auto &scomp = manager.getComponent<SpriteComponent>(speakerId);
	tgui::Texture tguiTex;
	tguiTex.load(scomp.tilesetPath, tgui::UIntRect({(unsigned)scomp.tileInfo.pixelX, (unsigned)scomp.tileInfo.pixelY},
	                                               {(unsigned)scomp.tileInfo.width, (unsigned)scomp.tileInfo.height}));
	speakerPicture->getRenderer()->setTexture(tguiTex);
}

void executeAction(ArchetypeManager &manager, DialogAction &action, EntityID &entityId)
{
	EntityID player = WorldUtils::getPlayer(manager).value();
	WorldComponent *world = WorldUtils::getWorld(manager);
	switch (action.action) {
	case DIALOG_ACTIONS::GET_ITEM: {
		auto &getItem = dynamic_cast<GET_ITEM_ACTION &>(action);
		CharacterComponent &ch = manager.getComponent<CharacterComponent>(player);
		manager.getComponent<PartOfLayerComponent>(getItem.itemId).groupId = ch.inventory.inventoryWorldId;
		world->pushMessageToHud("New item added!");
		break;
	}
	case DIALOG_ACTIONS::COMPANION: {
		CharacterComponent &ch = manager.getComponent<CharacterComponent>(player);
		manager.getComponent<PartOfLayerComponent>(entityId).groupId = ch.inventory.inventoryWorldId;
		if (ch.equipedCompanion == 0) {
			ch.equipedCompanion = entityId.getId();
		}
		world->pushMessageToHud("New Companion added!");
		break;
	}
	case DIALOG_ACTIONS::SWITCH_LAYER_DIALOG_ACTION: {
		auto &switchLayer = dynamic_cast<SwitchLayerAction &>(action);
		auto &layerinfo = manager.getComponent<PartOfLayerComponent>(switchLayer.destinationId);
		auto &transform = manager.getComponent<TransformComponent>(switchLayer.destinationId);
		manager.getComponent<PartOfLayerComponent>(player).groupId = layerinfo.groupId;
		manager.getComponent<TransformComponent>(player).position = transform.position;
		WorldUtils::getWorld(manager)->currentGroup = layerinfo.groupId;
		world->pushMessageToHud("...");
		break;
	}
	default:
		break;
	}
}

void createButton(ArchetypeManager &manager, DialogComponent &comp, DialogChoice &choice, int choiceIndex,
                  std::shared_ptr<tgui::PanelListBox> &box, EntityID &id)
{
	auto button = tgui::Button::create();
	button->setText(choice.text);
	button->setSize("100%", "100%");
	button->onPress([choiceIndex, &manager, &id, button]() {
		button->setEnabled(false);
		auto &dc = manager.getComponent<DialogComponent>(id);
		auto &choice = dc.current().choices[choiceIndex];
		if (choice.action && choice.action->action != DIALOG_ACTIONS::NO_ACTION) {
			executeAction(manager, *choice.action, id);
		}
		dc.advance(choiceIndex);
	});
	auto itemPanel = box->addItem();
	itemPanel->add(button);
}
void handleDialog(ArchetypeManager &manager, DialogComponent &comp, tgui::Gui &gui, EntityID &id)
{
	std::shared_ptr<tgui::Panel> dialog = getWidget<tgui::Panel>(gui, "DialogPanel");
	std::shared_ptr<tgui::Label> dialogNodeText = getWidget<tgui::Label>(gui, "SpeakerText");
	std::shared_ptr<tgui::PanelListBox> choicesBox = dialog->get<tgui::PanelListBox>("ChoicesBox");
	std::shared_ptr<tgui::Picture> speakerPicture = dialog->get<tgui::Picture>("SpeakerPicture");

	DialogNode &node = comp.current();
	if (node.speakerId > 0) {
		setPicture(manager, node.speakerId, speakerPicture);
	}
	dialogNodeText.get()->setText(node.text);

	int choiceIndex = 0;
	choicesBox->removeAllItems();
	for (auto &choice : node.choices) {
		createButton(manager, comp, choice, choiceIndex, choicesBox, id);
		choiceIndex++;
	}
	comp.currentNodeSent = true;
}

void DialogSystem::update()
{
	manager
	    .view<InteractionComponent, NPC_Component, DialogComponent, TransformComponent, RenderComponent,
	          SpriteComponent, PartOfLayerComponent>()
	    .each([&](auto &entity, InteractionComponent &interactioncomp, auto &npccomponent, DialogComponent &dialogComp,
	              auto &transform, auto &render, auto &sprite, auto &partOfLayer) {
		    if (!interactioncomp.isActive) {
			    if (hasActiveDialog && activeDialogEntity == entity) {
				    dialogComp.stop();
				    closeDialogPanelIfOpened(gui);
			    }
			    return;
		    }

		    if (dialogComp.isPassedOnce()) {
			    interactioncomp.isActive = false;
			    dialogComp.stop();
			    closeDialogPanelIfOpened(gui);
			    hasActiveDialog = false;
			    interactioncomp.deactivated = true;
			    return;
		    }
		    if (!hasActiveDialog) {
			    hasActiveDialog = true;
			    activeDialogEntity = entity;
		    }

		    if (activeDialogEntity != entity)
			    return;

		    if (dialogComp.currentNodeSent) {
			    return;
		    }
		    openPanelIfNotOpened(gui);
		    handleDialog(manager, dialogComp, gui, entity);
	    });
};
