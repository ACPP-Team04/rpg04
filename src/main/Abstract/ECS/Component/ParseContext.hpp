#pragma once
#include "tileson.h"
#include <algorithm>
struct ParseContext {
	int groupId;
	tson::Vector2i tileSize;
	const std::unique_ptr<tson::Map> &map;
};
