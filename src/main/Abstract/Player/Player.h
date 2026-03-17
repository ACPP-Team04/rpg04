#include <string>
class Player {
  public:
	Player(std::string name);
	virtual double size();
	virtual std::string getName();

  private:
	std::string name;
};