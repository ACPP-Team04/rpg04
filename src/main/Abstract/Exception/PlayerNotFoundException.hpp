#pragma once
#include <stdexcept>
class PlayerNotFoundException : public std::runtime_error {
  public:
	using std::runtime_error::runtime_error;
};