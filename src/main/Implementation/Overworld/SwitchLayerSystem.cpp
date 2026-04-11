#include "Abstract/Overwordl/SwitchLayerSystem.hpp"

#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/SwitchLayerComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

SwitchLayerSystem::SwitchLayerSystem(ArchetypeManager &manager) : System(manager) {}

void SwitchLayerSystem::update()
{
	WorldComponent *currentLayer = WorldUtils::getWorld(manager);
	if (!currentLayer)
		return;

	std::vector<std::pair<SwitchLayerComponent *, InteractionComponent *>> switchPoints;
	WorldUtils::viewInCurrentLayer<SwitchLayerComponent, InteractionComponent>(manager,
		[&](const EntityID &id, SwitchLayerComponent &lcomp, InteractionComponent &icomp) {
			if (icomp.action != INTERACTION_ACTION::SWITCH_LAYER) return;
			if (!icomp.isActive) return;
			switchPoints.emplace_back(&lcomp, &icomp);
		});

	if (switchPoints.empty())
		return;

	EntityID player = WorldUtils::getPlayer(manager).value();
	auto &partOfLayer = manager.getComponent<PartOfLayerComponent>(player);

	for (auto &[switchLayer, switchPos] : switchPoints) {
		int destination = switchLayer->destination;
		PartOfLayerComponent &destinationP = manager.getComponent<PartOfLayerComponent>(destination);
		TransformComponent &transform = manager.getComponent<TransformComponent>(destination);

		partOfLayer.layer = destinationP.layer;
		partOfLayer.level = destinationP.level;
		manager.getComponent<TransformComponent>(player).position = transform.position;
		currentLayer->currentLevel = destinationP.level;
		currentLayer->currentLayer = destinationP.layer;

		switchPos->isActive = false;
		switchPos->mustLeaveRadius = true;

		if (manager.hasComponent<SwitchLayerComponent>(destination)) {
			manager.getComponent<InteractionComponent>(destination).mustLeaveRadius = true;
		}
	}
}