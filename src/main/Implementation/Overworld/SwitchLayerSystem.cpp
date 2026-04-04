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
	CurrentLayerComponent *currentLayer = nullptr;
	this->manager.view<CurrentLayerComponent>().each(
	    [&](const EntityID &id, auto &component) { currentLayer = &component; });
	if (!currentLayer)
		return;

	std::vector<std::pair<SwitchLayerComponent *, TransformComponent *>> switchPoints;
	this->manager.view<SwitchLayerComponent, TransformComponent>().each(
	    [&](const EntityID &id, auto &component, auto &position) { switchPoints.push_back({&component, &position}); });

	if (switchPoints.empty())
		return;

	std::cout << "SwitchLayerSystem::switchPoints size: " << switchPoints.size() << std::endl;
	this->manager.view<InputComponent, PartOfLayerComponent, TransformComponent>().each(
	    [&](EntityID &id, auto &input, auto &partOfLayer, auto &transform) {
		    if (partOfLayer.layer != currentLayer->layer || partOfLayer.level != currentLayer->level)
			    return;

		    for (auto &[switchLayer, switchPos] : switchPoints) {
			    if (isNear(transform.position, switchPos->position)) {
				    currentLayer->level = switchLayer->level;
				    currentLayer->layer = switchLayer->layer;
				    partOfLayer.level = switchLayer->level;
				    partOfLayer.layer = switchLayer->layer;
				    std::cout << "Switched to: " << currentLayer->layer << " " << currentLayer->level << std::endl;
			    }
		    }
	    });
}