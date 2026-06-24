class PlayerComponentIncompleteException : public std::runtime_error {
  public:
	explicit PlayerComponentIncompleteException(const std::string &message) : std::runtime_error(message) {}
};