#pragma once

#include "Abstract/ECS/Component/ParseContext.hpp"
#include "tiled/Object.hpp"

struct DefaultComponent {
	virtual ~DefaultComponent() = default;

	virtual void readFromObject(tson::Object& object,ParseContext &context) = 0;
};