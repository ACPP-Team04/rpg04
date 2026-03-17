#include "Abstract/Player/Player.h"

Player::Player(std::string name)
{
	this->name = name;
};

std::string Player::getName()
{
	return this->name;
}

double Player::size()
{
	name = "Hallo";
	return 10;
}