#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

struct IsInInventoryComponent : Component<IsInInventoryComponent> {
	void readFromJson(tson::TiledClass &j) override {}
};