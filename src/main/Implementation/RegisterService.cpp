#include "Abstract/RegisterService.hpp"

#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/AnimationPartComponent.hpp"
#include "Abstract/Overwordl/Components/AudioComponent.hpp"
#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/CollisionComponent.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsLockedComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/MovementComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/Components/START_EQUIPMENT_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include "Abstract/Overwordl/Components/SwitchLayerComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"
#include "Components/BattleComponent.hpp"

void RegisterService::registerAudio() const
{
	AssetManager::getInstance().registerMusic("overworld", std::string(ROOT_DIR)
	                                                           + "/src/ressources/audio/music/the_field_of_dreams.ogg");
	AssetManager::getInstance().registerMusic("combat",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/music/battleThemeA.ogg");
	AssetManager::getInstance().registerMusic("lake_land",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/music/path_lake_land.ogg");

	AssetManager::getInstance().registerSound("victory_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/combat_victory.wav");
	AssetManager::getInstance().registerSound("defeat_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/death.wav");
	AssetManager::getInstance().registerSound("light_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/punch_1b.wav");
	AssetManager::getInstance().registerSound("heavy_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/punch_alt-2a.wav");
	AssetManager::getInstance().registerSound("ultimate_fist_hit",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/foom_0.wav");
	AssetManager::getInstance().registerSound("heal_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/healspell3.wav");
	AssetManager::getInstance().registerSound("rest_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/short_wind.wav");
	AssetManager::getInstance().registerSound("enemy_death_sound",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/zombie_death.wav");
	AssetManager::getInstance().registerSound("test",
	                                          std::string(ROOT_DIR) + "/src/ressources/audio/sfx/zombie_death.wav");
}

void RegisterService::registerComponents() const
{
	ComponentRegistry::getInstance().registerComponent<MovementComponent>("MOVEMENT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CameraComponent>("CAMERA_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<WorldComponent>("WORLD_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InputComponent>("INPUT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<SwitchLayerComponent>("SWITCH_LAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CollisionComponent>("COLLISION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<NPC_Component>("NPC_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<DialogComponent>("DIALOG_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<InteractionComponent>("INTERACTION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<PlayerComponent>("PLAYER_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ItemComponent>("ITEM_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<IsLockedComponent>("LOCKED_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<ITEM_HEALSTATS_COMPONENT>("ITEM_HEALSTATS_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BattleComponent>("BATTLE_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<BattleManagerComponent>("BattleManagerComponent");
	ComponentRegistry::getInstance().registerComponent<START_EQUIPMENT_COMPONENT>("EQUIPMENT_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<AudioComponent>("AUDIO_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<AnimationPartComponent>("ANIMATION_SPRITE_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<AnimationComponent>("ANIMATION_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<DialogComponent>("DIALOG_COMPONENT");
	ComponentRegistry::getInstance().registerComponent<CharacterComponent>("CharacterComponent");
}
