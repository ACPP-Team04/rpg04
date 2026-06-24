#pragma once
class MissingComponentException : public std::runtime_error {
  public:
	explicit MissingComponentException(const std::string &message) : std::runtime_error(message) {}
};