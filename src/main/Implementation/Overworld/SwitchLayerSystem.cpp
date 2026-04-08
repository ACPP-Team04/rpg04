#include "Abstract/Overwordl/SwitchLayerSystem.hpp"

#include "Abstract/Overwordl/Components.hpp"

SwitchLayerSystem::SwitchLayerSystem(ArchetypeManager &manager) : System(manager) {}

static bool isNear(sf::Vector2f a, sf::Vector2f b, float radius = 16.0f)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return (dx * dx + dy * dy) <= (radius * radius);
}
void SwitchLayerSystem::update()
{
	WorldComponent *currentLayer = nullptr;
	this->manager.view<WorldComponent>().each([&](const EntityID &id, auto &component) { currentLayer = &component; });
	if (!currentLayer)
		return;

	std::vector<std::pair<SwitchLayerComponent *, TransformComponent *>> switchPoints;
	this->manager.view<SwitchLayerComponent, TransformComponent>().each(
	    [&](const EntityID &id, auto &component, auto &position) { switchPoints.push_back({&component, &position}); });

	if (switchPoints.empty())
		return;

	this->manager.view<InputComponent, PartOfLayerComponent, TransformComponent>().each(
	    [&](EntityID &id, auto &input, auto &partOfLayer, auto &transform) {
		    if (partOfLayer.layer != currentLayer->currentLayer || partOfLayer.level != currentLayer->currentLevel)
			    return;

		    for (auto &[switchLayer, switchPos] : switchPoints) {
			    if (isNear(transform.position, switchPos->position)) {
				    currentLayer->currentLevel = switchLayer->level;
				    currentLayer->currentLayer = switchLayer->layer;
				    partOfLayer.level = switchLayer->level;
				    partOfLayer.layer = switchLayer->layer;
			    }
		    }
	    });
}