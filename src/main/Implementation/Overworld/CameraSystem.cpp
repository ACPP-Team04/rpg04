#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

CameraSystem::CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};


sf::Vector2f getCameraSize(WorldComponent &world, CameraComponent &camera)
{
	return {(float)world.widthPixel * camera.scaleSize.x, (float)world.heightPixel * camera.scaleSize.y};
}
void CameraSystem::update()
{
	WorldComponent *world = WorldUtils::getWorld(manager);
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(manager,
	    [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;
		    sf::View cameraView;
		    cameraView.setCenter(camera.center);
		    cameraView.setSize(getCameraSize(*world, camera));
		    window.setView(cameraView);
	    });
}