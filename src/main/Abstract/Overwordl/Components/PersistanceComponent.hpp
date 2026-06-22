#include "Abstract/ECS/Component/Component.hpp"

struct PersistanceComponent : public Component<PersistanceComponent> {
	std::string uuid;

	void readFromJson(tson::TiledClass &j)
	{
		// Intentionally empty:
		// This component is strictly internal and is not parsed directly from Tiled
	}
};