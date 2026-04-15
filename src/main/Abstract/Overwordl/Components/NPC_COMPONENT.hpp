#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct NPC_Component : public Component<NPC_Component> {

	void readFromJson(tson::TiledClass &j) override{}

};
