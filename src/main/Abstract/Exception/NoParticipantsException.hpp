#pragma once
class NoParticipantsException : public std::runtime_error {
  public:
	explicit NoParticipantsException(const std::string &message) : std::runtime_error(message) {}
};