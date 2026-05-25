#include "Abstract/Overwordl/CameraSystem.hpp"

#include "Abstract/Overwordl/Components/CameraComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

CameraSystem::CameraSystem(ArchetypeManager &manager, sf::RenderWindow &window) : System(manager), window(window) {};

void CameraSystem::update()
{
	WorldUtils::viewInCurrentLayer<CameraComponent, TransformComponent>(
	    manager, [&](EntityID &e, CameraComponent &camera, TransformComponent &transform) {
		    camera.center = transform.position;
		    sf::View cameraView;
		    cameraView.setCenter(camera.center);
		    cameraView.setSize(
		        {(float)window.getSize().x * camera.scaleSize.x, (float)window.getSize().y * camera.scaleSize.y});
		    window.setView(cameraView);
	    });
}