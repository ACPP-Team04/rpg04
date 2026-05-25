#pragma once

#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "DefaultComponent.hpp"
class StateComponent : Component<StateComponent>,DefaultComponent {
  private:
	ENTITY_ANIMATIONS_STATE state =IDLE;
	bool locked = false;

  public:
	void readFromJson(tson::TiledClass &j) override {}
	void readFromObject(tson::Object &object, ParseContext &context) override{};

	void setState(ENTITY_ANIMATIONS_STATE newState, bool lock = false)
	{
		if (locked)
			return;
		state = newState;
		locked = lock;
	}

	ENTITY_ANIMATIONS_STATE getState() const { return state; }
	bool isLocked() const { return locked; }
};