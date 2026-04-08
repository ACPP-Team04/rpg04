#include "Abstract/Overwordl/SwitchLayerSystem.hpp"

#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/SwitchLayerComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"

SwitchLayerSystem::SwitchLayerSystem(ArchetypeManager &manager) : System(manager) {}

void SwitchLayerSystem::update()
{
	WorldComponent *currentLayer = nullptr;
	this->manager.view<WorldComponent>().each([&](const EntityID &id, auto &component) { currentLayer = &component; });
	if (!currentLayer)
		return;

	std::vector<std::pair<SwitchLayerComponent *, InteractionComponent>> switchPoints;

	this->manager.view<SwitchLayerComponent, InteractionComponent>().each(
	    [&](const EntityID &id, SwitchLayerComponent &lcomp, InteractionComponent &icomp) {
		    if (lcomp.layer == currentLayer->currentLayer && lcomp.level == currentLayer->currentLevel) {
			    return;
		    }
		    if (icomp.action != INTERACTION_ACTION::SWITCH_LAYER) {
			    return;
		    }
		    if (!icomp.isActive) {
			    return;
		    }

		    switchPoints.emplace_back(&lcomp, icomp);
	    });

	if (switchPoints.empty())
		return;

	this->manager.view<PlayerComponent, PartOfLayerComponent>().each(
	    [&](EntityID &id, auto &player, auto &partOfLayer) {
		    if (partOfLayer.layer != currentLayer->currentLayer || partOfLayer.level != currentLayer->currentLevel)
			    return;

		    for (auto &[switchLayer, switchPos] : switchPoints) {
			    currentLayer->currentLevel = switchLayer->level;
			    currentLayer->currentLayer = switchLayer->layer;
			    partOfLayer.level = switchLayer->level;
			    partOfLayer.layer = switchLayer->layer;
		    }
	    });
}