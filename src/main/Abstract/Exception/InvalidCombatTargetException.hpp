#pragma once
#include <stdexcept>
#include <string>

class InvalidCombatTargetException : public std::runtime_error {
  public:
	explicit InvalidCombatTargetException(const std::string &message) : std::runtime_error(message) {}
};