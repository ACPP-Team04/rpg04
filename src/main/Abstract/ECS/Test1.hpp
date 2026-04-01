#pragma once
#include "Component/Component.hpp"

class Test1 : public Component<Test1> {
  public:
	Test1() = default;

	int x{};
	int y{};
	void readFromJson(const nlohmann::json &j){

	}
};

class Test2 : public Component<Test2> {
  public:
	Test2() = default;

	int x{};
	int y{};
	void readFromJson(const nlohmann::json &j){

	}
};