#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

CameraSystem::CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void CameraSystem::setTargetAspect(float newTargetAspect)
{
	targetAspect = newTargetAspect;
}

void CameraSystem::setViewport(const sf::FloatRect &newViewport)
{
	viewport = newViewport;
}

sf::Vector2f getCameraSize(WorldComponent &world, CameraComponent &camera, float targetAspect)
{
	sf::Vector2f cameraSize{(float)world.widthPixel * camera.scaleSize.x,
	                        (float)world.heightPixel * camera.scaleSize.y};
	const float cameraAspect = cameraSize.x / cameraSize.y;

	if (cameraAspect > targetAspect) {
		cameraSize.x = cameraSize.y * targetAspect;
	} else if (cameraAspect < targetAspect) {
		cameraSize.y = cameraSize.x / targetAspect;
	}

	return cameraSize;
}

void CameraSystem::update()
{
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(
	    manager, [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;
		    sf::View cameraView;
		    cameraView.setCenter(camera.center);
		    cameraView.setViewport(viewport);
		    cameraView.setSize(
		        {(float)window.getSize().x * camera.scaleSize.x, (float)window.getSize().y * camera.scaleSize.y});
		    window.setView(cameraView);
	    });
}
