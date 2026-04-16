#pragma once
#include "Component/Component.hpp"

class Test1 : public Component<Test1> {
  public:
	Test1() = default;

	int x{};
	int y{};
	virtual void readFromJson(tson::TiledClass &j) override{}
};

class Test2 : public Component<Test2> {
  public:
	Test2() = default;

	int x{};
	int y{};
	virtual void readFromJson(tson::TiledClass &j) override {}
};