#pragma once
class SaveSlotNotFoundException : public std::runtime_error {
  public:
	explicit SaveSlotNotFoundException(const std::string &message) : std::runtime_error(message) {}
};