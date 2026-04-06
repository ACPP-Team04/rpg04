#pragma once
#include "Abstract/AssetManager/AssetManager.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Components.hpp"

#include <SFML/Graphics/Sprite.hpp>

struct CollisionSystem : public System {
	CollisionSystem(ArchetypeManager &manager);
	void update() override;
};