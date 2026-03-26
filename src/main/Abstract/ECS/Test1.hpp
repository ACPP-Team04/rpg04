#pragma once
#include "Abstract/ECS/Component.hpp"


class Test1 : public Component {
public:
	Test1();

	int x;
	int y;
};