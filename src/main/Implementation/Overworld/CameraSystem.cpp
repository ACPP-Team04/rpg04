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

sf::Vector2f getCameraSize(CameraComponent &camera, sf::RenderWindow &window)
{
	float windowAspect = (float)window.getSize().x / (float)window.getSize().y;
	float height = BaseCameraSize.y * camera.scaleSize.y;
	float width = height * windowAspect;
	return {width, height};
}

void CameraSystem::update()
{
	WorldComponent *world = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(
	    manager, [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;
		    sf::Vector2f cameraSize = getCameraSize(camera, window);
	    	sf::Vector2f halfSize = cameraSize / 2.f;float minX = halfSize.x;
			float minY = halfSize.y;
			float maxX = (float)world->widthPixel - halfSize.x;
			float maxY = (float)world->heightPixel - halfSize.y;
	    	camera.center.x = std::clamp(camera.center.x, minX, maxX);
	    	camera.center.y = std::clamp(camera.center.y, minY, maxY);
		    sf::View cameraView;
		    cameraView.setSize(cameraSize);
		    cameraView.setCenter(camera.center);
	    	cameraView.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
		    window.setView(cameraView);
	    });
}
