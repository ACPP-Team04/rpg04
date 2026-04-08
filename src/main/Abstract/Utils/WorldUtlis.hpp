#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

class WorldUtils {
  public:
	static bool isCurrentLayer(ArchetypeManager &manager, LAYERTYPE targetType)
	{
		WorldComponent *world = nullptr;
		manager.view<WorldComponent>().each([&](auto id, auto &comp) { world = &comp; });
		return (world && world->currentLayer == targetType);
	}
};
