#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

CameraSystem::CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void CameraSystem::update()
{
	WorldComponent world;

	this->manager.view<WorldComponent>().each(
	    [&](const EntityID &entity_id, WorldComponent &component) { world = component; });
	this->manager.view<CameraComponent, TransformComponent>().each(
	    [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, e)) {
			    return;
		    }
		    camera.center = transform.position;

		    sf::View cameraView;
		    cameraView.setCenter(camera.center);
		    cameraView.setSize(
		        {(float)world.widthPixel * camera.scaleSize.x, (float)world.heightPixel * camera.scaleSize.y});
		    window.setView(cameraView);
	    });
}