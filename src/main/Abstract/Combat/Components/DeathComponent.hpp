#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
struct DeathComponent : public Component<DeathComponent> {
	DeathComponent() = default;
	virtual void readFromJson(tson::TiledClass &j) override {
		// Intentionally empty:
		// This component is strictly internal and is not parsed directly from Tiled
	};
	TileInfo graveTile;
	std::string graveTilesetPath;
};
