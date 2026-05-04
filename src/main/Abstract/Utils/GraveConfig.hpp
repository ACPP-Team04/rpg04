#pragma once
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
#include <string>
struct GraveConfig {
	static inline TileInfo defaultTile = {16,160,16,16};
	static inline std::string tilesetPath = SPRITE_SHEET_PATH;
};