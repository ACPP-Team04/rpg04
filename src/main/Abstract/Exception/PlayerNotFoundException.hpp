#pragma once
class PlayerNotFoundException : public std::runtime_error {
  public:
	explicit PlayerNotFoundException(const std::string &message) : std::runtime_error(message) {}
};