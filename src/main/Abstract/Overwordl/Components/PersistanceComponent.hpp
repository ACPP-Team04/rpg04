#include "Abstract/ECS/Component/Component.hpp"

struct PersistanceComponent : public Component<PersistanceComponent> {
	std::string uuid;

	void readFromJson(tson::TiledClass &j) {}
};