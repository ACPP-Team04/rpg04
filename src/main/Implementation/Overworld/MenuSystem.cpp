
#include "Abstract/Overwordl/MenuSystem.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <Abstract/Overwordl/Components/ItemComponent.hpp>
#include <Abstract/Overwordl/Components/SpriteComponent.hpp>
#include <Abstract/Persistance/SaveManager.hpp>
#include <Implementation/Components/WeaponComponent.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iomanip>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
enum class MENU_TAB { WEAPONS, COLLECTABLES, COMPANIONS, STATS };

MenuSystem::MenuSystem(ArchetypeManager &manager, tgui::Gui &gui) : System(manager), gui(gui) {};

void overworldMenu(ArchetypeManager &manager, WorldComponent *world, tgui::Gui &gui)
{
	if (gui.get("overworldMenu"))
		return;

	auto panel = tgui::Panel::create({"300", "250"});
	panel->setWidgetName("overworldMenu");
	panel->setPosition({"50%", "50%"});
	panel->setOrigin(0.5f, 0.5f);
	panel->getRenderer()->setBackgroundColor(tgui::Color(30, 30, 30, 240));
	panel->getRenderer()->setBorders({2});
	panel->getRenderer()->setBorderColor(tgui::Color::White);

	float yPos = 30.f;

	auto resumeBtn = tgui::Button::create("Resume Game");
	resumeBtn->setSize({"80%", 40});
	resumeBtn->setPosition({"10%", yPos});
	resumeBtn->onClick([world, &gui]() {
		world->menuOpened = false;
		gui.remove(gui.get("overworldMenu"));
	});
	panel->add(resumeBtn);

	yPos += 60.f;

	auto loadBtn = tgui::Button::create("Load Game (Slot 1)");
	loadBtn->setSize({"80%", 40});
	loadBtn->setPosition({"10%", yPos});

	if (!SaveManager::doesSaveExist(1)) {
		loadBtn->setText("No Save Found");
		loadBtn->setEnabled(false);
	} else {
		loadBtn->onClick([world, &gui]() {
			spdlog::info("Overworld Menu: Load Game requested for Slot 1");
			PersistenceManager::getInstance().requestLoad = true;
			world->menuOpened = false;
			gui.remove(gui.get("overworldMenu"));
		});
	}
	panel->add(loadBtn);

	yPos += 60.f;

	auto exitBtn = tgui::Button::create("Exit Game");
	exitBtn->setSize({"80%", 40});
	exitBtn->setPosition({"10%", yPos});
	exitBtn->onClick([world, &gui]() {
		spdlog::info("Overworld Menu: Safe Shutdown Requested...");
		// TODO: Do we want auto save on exit?
		// PersistenceManager::getInstance().requestSave = true;
		PersistenceManager::getInstance().requestQuit = true;
		world->menuOpened = false;
		gui.remove(gui.get("overworldMenu"));
	});
	panel->add(exitBtn);

	gui.add(panel);
}
void openMenu(ArchetypeManager &manager, WorldComponent *world, tgui::Gui &gui)
{
	LAYERTYPE layertype = world->currentLayer;
	world->menuOpened = true;

	if (layertype == LAYERTYPE::OVERWORLD) {
		overworldMenu(manager, world, gui);
	}
}

void buildInventoryMenu(ArchetypeManager &manager, WorldComponent *world, tgui::Gui &gui,
                        MENU_TAB activeTab = MENU_TAB::WEAPONS)
{
	if (gui.get("inventoryMenu"))
		return;

	auto mainPanel = tgui::Panel::create({"80%", "80%"});
	mainPanel->setWidgetName("inventoryMenu");
	mainPanel->setPosition({"10%", "10%"});
	mainPanel->getRenderer()->setBackgroundColor(tgui::Color(30, 30, 30, 240));

	auto tabs = tgui::Tabs::create();
	tabs->setPosition({"0%", "0%"});
	tabs->setSize({"100%", 40});

	tabs->add("Weapons");
	tabs->add("Collectables");
	tabs->add("Companions");
	tabs->add("Stats");

	if (activeTab == MENU_TAB::WEAPONS)
		tabs->select("Weapons");
	else if (activeTab == MENU_TAB::COLLECTABLES)
		tabs->select("Collectables");
	else if (activeTab == MENU_TAB::COMPANIONS)
		tabs->select("Companions");
	else if (activeTab == MENU_TAB::STATS)
		tabs->select("Stats");

	tabs->onTabSelect([&manager, world, &gui](const tgui::String &activeTab) {
		MENU_TAB newTab = MENU_TAB::WEAPONS;
		if (activeTab == "Stats")
			newTab = MENU_TAB::STATS;
		else if (activeTab == "Collectables")
			newTab = MENU_TAB::COLLECTABLES;
		else if (activeTab == "Companions")
			newTab = MENU_TAB::COMPANIONS;

		gui.remove(gui.get("inventoryMenu"));
		buildInventoryMenu(manager, world, gui, newTab);
	});
	mainPanel->add(tabs);

	auto itemList = tgui::ScrollablePanel::create({"40%", "100% - 40"});
	itemList->setPosition({"0%", 40});
	itemList->getRenderer()->setBackgroundColor(tgui::Color(20, 20, 20));
	mainPanel->add(itemList);

	auto inspectorPanel = tgui::Panel::create({"60%", "100%- 40"});
	inspectorPanel->setPosition({"40%", 40});
	inspectorPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
	mainPanel->add(inspectorPanel);

	if (activeTab == MENU_TAB::STATS) {
		auto statsPanel = tgui::Panel::create({"100%", "100% - 40"});
		statsPanel->setPosition({"0%", 40});
		statsPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
		mainPanel->add(statsPanel);

		manager.view<PlayerComponent, CharacterComponent>().each([&statsPanel](auto &entity, auto &player,
		                                                                       auto &characterComp) {
			auto numFightsLabel =
			    tgui::Label::create("Number of Fights Won: " + std::to_string(characterComp.stats.numberOfFightsWon));
			numFightsLabel->setTextSize(30);
			numFightsLabel->setPosition({"10%", "10%"});
			statsPanel->add(numFightsLabel);

			std::array<std::string, 4> statNames = {"Strength", " Dexterity", "Faith", "Max_health"};

			float startY = 130.f;
			float rowHeight = 40.f;
			float spacing = 10.f;
			int rowIndex = 0;

			for (auto &[statEnum, statNameStrView] : magic_enum::enum_entries<STATS>()) {

				std::string statName = std::string(statNameStrView);

				int statValue = characterComp.stats.stats[static_cast<size_t>(statEnum)];

				auto statRow = tgui::Panel::create({"40%", rowHeight});
				statRow->setPosition({"10%", startY + (rowIndex * (rowHeight + spacing))});
				statRow->getRenderer()->setBackgroundColor(tgui::Color(40, 40, 40));
				statRow->getRenderer()->setBorders({1});
				statRow->getRenderer()->setBorderColor(tgui::Color(80, 80, 80));
				statsPanel->add(statRow);

				auto nameLabel = tgui::Label::create(statName);
				nameLabel->setPosition({10, 10});
				nameLabel->setTextSize(18);
				nameLabel->getRenderer()->setTextColor(tgui::Color::White);
				statRow->add(nameLabel);

				auto valueLabel = tgui::Label::create(std::to_string(statValue));
				valueLabel->setPosition({"100% - width - 15", 10});

				valueLabel->setTextSize(18);
				valueLabel->getRenderer()->setTextColor(tgui::Color::Yellow);
				statRow->add(valueLabel);

				rowIndex++;
			}
		});

	} else {

		InventoryComponent *inventory = nullptr;
		auto playerOpt = WorldUtils::getPlayer(manager);
		if (!playerOpt.has_value()) {
			throw std::runtime_error("No player found in the current layer to show inventory menu");
		}
		CharacterComponent &characterComp = manager.getComponent<CharacterComponent>(playerOpt.value());
		ITEM_TYPE activeTabItemType = ITEM_TYPE::WEAPON;
		if (activeTab == MENU_TAB::COLLECTABLES) {
			activeTabItemType = ITEM_TYPE::COLLECTABLE;
		} else if (activeTab == MENU_TAB::COMPANIONS) {
			activeTabItemType = ITEM_TYPE::COLLECTABLE_COMPANION;
		}
		std::vector<EntityID> itemsInInventory;

		WorldUtils::viewInSpecificLayer<ItemComponent>(
		    manager, characterComp.inventory.inventoryWorldId,
		    [&activeTabItemType, &itemsInInventory](EntityID entity, ItemComponent &itemComp) {
			    if (itemComp.itemType == activeTabItemType) {
				    itemsInInventory.push_back(entity);
			    }
		    });

		if (!itemsInInventory.empty()) {
			float yOffset = 10.f;

			EntityID playerId = playerOpt.value();

			for (EntityID itemEntity : itemsInInventory) {

				if (!manager.hasComponent<ItemComponent>(itemEntity)) {
					spdlog::error("Entity {} in inventory does not have an ItemComponent", itemEntity.getId());
					continue;
				}

				auto &itemComp = manager.getComponent<ItemComponent>(itemEntity);
				std::string itemName = itemComp.name;

				bool isEquipped = (itemEntity.getId() == characterComp.equipedWeapon
				                   || itemEntity.getId() == characterComp.equipedCompanion);

				std::string btnText = "Item: " + itemName;
				if (isEquipped) {
					btnText += " [EQUIPPED]";
				}

				auto btn = tgui::Button::create(btnText);
				btn->setSize({"90%", 40});
				btn->setPosition({"5%", yOffset});

				if (isEquipped) {
					btn->getRenderer()->setTextColor(tgui::Color::Green);
				}

				btn->onClick([&manager, world, &gui, itemEntity, inspectorPanel, activeTabItemType, itemName,
				              isEquipped, playerId]() {
					inspectorPanel->removeAllWidgets();

					auto title = tgui::Label::create("Inspecting " + itemName);
					title->setTextSize(24);
					title->setPosition({20, 20});
					title->getRenderer()->setTextColor(tgui::Color::White);
					inspectorPanel->add(title);

					auto imageBox = tgui::Panel::create({"100", "100"});
					imageBox->setPosition({280, 20});
					imageBox->getRenderer()->setBackgroundColor(tgui::Color(15, 15, 15));
					imageBox->getRenderer()->setBorders({2});
					imageBox->getRenderer()->setBorderColor(tgui::Color(100, 100, 100));
					inspectorPanel->add(imageBox);

					if (manager.hasComponent<SpriteComponent>(itemEntity)) {
						auto &spriteComp = manager.getComponent<SpriteComponent>(itemEntity);

						try {
							sf::Sprite itemSprite = AssetManager::getInstance().getSpriteAt(spriteComp);
							auto canvas = tgui::CanvasSFML::create({"100%", "100%"});
							imageBox->add(canvas);
							canvas->clear(tgui::Color::Transparent);

							sf::FloatRect bounds = itemSprite.getLocalBounds();
							if (bounds.size.x > 0 && bounds.size.y > 0) {
								itemSprite.setScale({100.f / bounds.size.x, 100.f / bounds.size.y});
							}
							canvas->draw(itemSprite);
							canvas->display();

						} catch (const std::exception &e) {
							spdlog::warn("Could not load sprite for item {}: {}", itemName, e.what());
						}
					} else {
						auto noSpriteLabel = tgui::Label::create("?");
						noSpriteLabel->setTextSize(50);
						noSpriteLabel->setPosition({"35%", "15%"});
						noSpriteLabel->getRenderer()->setTextColor(tgui::Color(80, 80, 80));
						imageBox->add(noSpriteLabel);
					}

					if (activeTabItemType == ITEM_TYPE::WEAPON) {
						auto &weapon = manager.getComponent<ItemComponent>(itemEntity).weaponStats;
						std::stringstream stream;
						stream << std::fixed << std::setprecision(2) << weapon.getScalingFactor();
						std::string formattedScaling = stream.str();
						std::string enumNameScalingFactor = std::string(magic_enum::enum_name(weapon.scalingFactor));
						std::string enumNameWeaponType = std::string(magic_enum::enum_name(weapon.weaponType));
						std::string enumNameScalingStat = std::string(magic_enum::enum_name(weapon.scalingStat));

						auto statsLabel = tgui::Label::create(
						    "Light Attack: " + std::to_string(weapon.lightAttackBaseDmg) + "\n"
						    + "Heavy Attack: " + std::to_string(weapon.heavyAttackBaseDmg) + "\n"
						    + "Scaling Factor: " + enumNameScalingFactor + " (" + formattedScaling + ")\n"
						    + "Scaling Stat: " + enumNameScalingStat + "\n" + "Weapon Type: " + enumNameWeaponType);

						statsLabel->setPosition({20, 70});
						statsLabel->getRenderer()->setTextColor(tgui::Color::Yellow);
						inspectorPanel->add(statsLabel);
					}
					if (activeTabItemType == ITEM_TYPE::COLLECTABLE_COMPANION) {
						if (!manager.hasComponent<CharacterComponent>(itemEntity)) {
							spdlog::error("Companion item {} does not have a CharacterComponent for its stats",
							              itemName);
							return;
						}

						auto &compStats = manager.getComponent<CharacterComponent>(itemEntity).stats;

						std::string statsText = "HP: " + std::to_string((int)compStats.health) + " / "
						                        + std::to_string(compStats.getStat(STATS::MAX_HEALTH)) + "\n\n"
						                        + "Strength: " + std::to_string(compStats.getStat(STATS::STRENGTH))
						                        + "\n"
						                        + "Dexterity: " + std::to_string(compStats.getStat(STATS::DEXTERITY))
						                        + "\n" + "Faith: " + std::to_string(compStats.getStat(STATS::FAITH));

						auto statsLabel = tgui::Label::create(statsText);
						statsLabel->setPosition({20, 70});
						statsLabel->getRenderer()->setTextColor(tgui::Color::Yellow);

						inspectorPanel->add(statsLabel);
					}
					if (activeTabItemType == ITEM_TYPE::WEAPON
					    || activeTabItemType == ITEM_TYPE::COLLECTABLE_COMPANION) {
						auto equipBtn = tgui::Button::create();
						equipBtn->setSize({150, 50});
						equipBtn->setPosition({20, 200});

						if (isEquipped) {
							equipBtn->setText("EQUIPPED");
							equipBtn->setEnabled(false);
						} else {
							equipBtn->setText("EQUIP");

							equipBtn->onClick([&manager, world, &gui, itemEntity, activeTabItemType, playerId]() {
								CharacterComponent &dynamicCharComp =
								    manager.getComponent<CharacterComponent>(playerId);

								if (activeTabItemType == ITEM_TYPE::WEAPON) {
									dynamicCharComp.equipedWeapon = itemEntity.getId();
									spdlog::info("Weapon Equipped!");
								} else if (activeTabItemType == ITEM_TYPE::COLLECTABLE_COMPANION) {
									dynamicCharComp.equipedCompanion = itemEntity.getId();
									spdlog::info("Companion Equipped!");
								}

								auto oldMenu = gui.get("inventoryMenu");
								if (oldMenu) {
									gui.remove(oldMenu);
								}
								buildInventoryMenu(manager, world, gui);
							});
						}

						inspectorPanel->add(equipBtn);
					}
				});

				itemList->add(btn);
				yOffset += 50.f;
			}
		}
	}
	gui.add(mainPanel);
}

void MenuSystem::update()
{
	WorldComponent *world = nullptr;
	this->manager.view<WorldComponent>().each([&world](auto &entity, auto &component) { world = &component; });
	InputComponent *input = nullptr;
	this->manager.view<InputComponent>().each(
	    [&input](auto &entity, auto &inputComponent) { input = &inputComponent; });
	if (!world || !input) {
		return;
	}
	if (input->menuButton.justPressed) {
		if (!world->menuOpened) {
			openMenu(manager, world, this->gui);
		} else {
			world->menuOpened = false;
			if (gui.get("overworldMenu")) {
				gui.remove(gui.get("overworldMenu"));
			}
		}
	}
	if (input->inventoryButton.justPressed) {
		if (!world->menuOpened) {
			world->menuOpened = true;
			world->currentMenu = ACTIVE_MENU::INVENTORY_MENU;
			buildInventoryMenu(manager, world, this->gui);
		} else if (world->currentMenu == ACTIVE_MENU::INVENTORY_MENU) {
			world->menuOpened = false;
			world->currentMenu = ACTIVE_MENU::NONE;
			if (gui.get("inventoryMenu")) {
				gui.remove(gui.get("inventoryMenu"));
			}
		}
	}
}