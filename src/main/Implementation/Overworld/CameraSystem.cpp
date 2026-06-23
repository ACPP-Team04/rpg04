#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Abstract/GlobalProperties.hpp"

namespace {
constexpr sf::Vector2f BaseCameraSize{WORLD_SIZE_X, WORLD_SIZE_Y};
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

sf::Vector2f getCameraSize(const CameraComponent &camera)
{
	return {BaseCameraSize.x * camera.scaleSize.x, BaseCameraSize.y * camera.scaleSize.y};
}

void CameraSystem::update()
{
	WorldComponent *world = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(
	    manager, [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;
		    sf::Vector2f cameraSize = getCameraSize(camera);
	    	sf::Vector2f halfSize = cameraSize / 2.f;
			float minX = halfSize.x;
			float minY = halfSize.y;
			float maxX = (float)world->widthPixel - halfSize.x;
			float maxY = (float)world->heightPixel - halfSize.y;
	    	camera.center.x = std::clamp(camera.center.x, minX, maxX);
	    	camera.center.y = std::clamp(camera.center.y, minY, maxY);
		    sf::View cameraView;
		    cameraView.setSize(cameraSize);
		    cameraView.setCenter(camera.center);
	    	cameraView.setViewport(viewport);
		    window.setView(cameraView);
	    });
}
