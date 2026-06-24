#pragma once
#include "Abstract/ECS/System/System.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

struct SwitchLayerSystem : System {

	explicit SwitchLayerSystem(ArchetypeManager &manager);
	void update() override;
};