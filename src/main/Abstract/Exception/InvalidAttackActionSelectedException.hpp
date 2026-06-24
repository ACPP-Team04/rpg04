#pragma once
class InvalidAttackActionSelectedException : public std::runtime_error {
  public:
	using std::runtime_error::runtime_error;
};