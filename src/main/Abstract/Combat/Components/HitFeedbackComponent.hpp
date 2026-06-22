#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct HitFeedbackComponent : public Component<HitFeedbackComponent> {
	int framesElapsed = 0;
	int totalFrames = 36;
	int blinkInterval = 6;

	virtual void readFromJson(tson::TiledClass &j) override {
		// Intentionally empty:
		// This component is strictly internal and is not parsed directly from Tiled
	};
};