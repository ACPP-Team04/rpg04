#pragma once
class InvalidAttackActionSelectedException : public std::runtime_error {
  public:
	explicit InvalidAttackActionSelectedException(const std::string &message) : std::runtime_error(message) {}
};