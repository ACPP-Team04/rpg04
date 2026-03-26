#pragma once
#include <string>
class Player {
  public:
	virtual ~Player() = default;
	Player(std::string name);
	virtual double size();
	virtual std::string getName();

  private:
	std::string name;
};