#include "Abstract/Overwordl/BonfireSystem.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Persistance/SaveManager.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <spdlog/spdlog.h>

BonfireSystem::BonfireSystem(ArchetypeManager &manager) : System(manager) {}

void BonfireSystem::update()
{
	WorldUtils::viewInCurrentLayer<InteractionComponent>(manager, [&](auto entity, InteractionComponent &component) {
		if (!component.isActive) {
			return;
		}

		if (component.action == INTERACTION_ACTION::BONFIRE_REST) {

			spdlog::info("Bonfire interaction detected! Executing save...");

			// FIX: Hardcoded for now slot 1
			SaveManager::saveGame(manager, 1);

			component.isActive = false;
		}
	});
}