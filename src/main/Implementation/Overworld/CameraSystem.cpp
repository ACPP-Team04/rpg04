#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

namespace {
constexpr sf::Vector2f BaseCameraSize{800.f, 600.f};
}

CameraSystem::CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void CameraSystem::setTargetAspect(float newTargetAspect)
{
	targetAspect = newTargetAspect;
}

void CameraSystem::setViewport(const sf::FloatRect &newViewport)
{
	viewport = newViewport;
}

sf::Vector2f getCameraSize(CameraComponent &camera, float targetAspect)
{
	sf::Vector2f cameraSize{BaseCameraSize.x * camera.scaleSize.x, BaseCameraSize.y * camera.scaleSize.y};
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
	WorldComponent *world = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(
	    manager, [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;

		    sf::View cameraView;
		    /*
	    	cameraView.setSize(
		        {(float)window.getSize().x * camera.scaleSize.x, (float)window.getSize().y * camera.scaleSize.y});

		    sf::Vector2f halfSize = cameraView.getSize() / 2.f;
		    float minX = halfSize.x;
		    float minY = halfSize.y;
		    float maxX = (float)world->widthPixel - halfSize.x;
		    float maxY = (float)world->heightPixel - halfSize.y;

		    if (maxX > minX && maxY > minY) {
			    camera.center.x = std::clamp(camera.center.x, minX, maxX);
			    camera.center.y = std::clamp(camera.center.y, minY, maxY);
		    }
			*/
		    cameraView.setCenter(camera.center);
		    cameraView.setViewport(viewport);
		    cameraView.setSize(getCameraSize(camera, targetAspect));
		    window.setView(cameraView);
	    });
}
