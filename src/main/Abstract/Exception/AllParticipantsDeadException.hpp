#pragma once
class AllParticipantsDeadException : public std::runtime_error {
  public:
	explicit AllParticipantsDeadException(const std::string &message) : std::runtime_error(message) {}
};