#pragma once
#include <stdexcept>
#include <string>

class InvalidCombatTargetException : public std::runtime_error {
  public:
	using std::runtime_error::runtime_error;
};