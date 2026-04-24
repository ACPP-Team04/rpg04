#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
struct DeathComponent : public Component<DeathComponent> {
	DeathComponent() = default;
	virtual void readFromJson(tson::TiledClass &j) override {};
	TileInfo graveTile;
	std::string graveTilesetPath;
};
