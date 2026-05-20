#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
class StateComponent: Component <StateComponent> {
private:
	ENTITY_ANIMATIONS_STATE state;
	bool locked = false;
public:
	void readFromJson(tson::TiledClass &j)override
	{

	}

	void setState(ENTITY_ANIMATIONS_STATE newState, bool lock = false) {
		if (locked) return;
		state = newState;
		locked = lock;
	}

	ENTITY_ANIMATIONS_STATE getState() const {
		return state;
	}
	bool isLocked() const
	{
		return locked;
	}


};