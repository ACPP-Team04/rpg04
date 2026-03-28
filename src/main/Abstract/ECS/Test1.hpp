#pragma once
#include "Abstract/ECS/Component.hpp"


class Test1 : public Component {
public:
	Test1()= default;

	int x{};
	int y{};
};

class Test2 : public Component {
public:
	Test2()= default;

	int x{};
	int y{};
};