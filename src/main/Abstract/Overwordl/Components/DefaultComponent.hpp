#pragma once
#include "Abstract/ECS/Component/ComponentRegistry.hpp"
#include "tiled/Object.hpp"

struct DefaultComponent {

	virtual void readFromObject(tson::Object& object,ParseContext &context) = 0;
};