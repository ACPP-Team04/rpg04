#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct HitFeedbackComponent : public Component<HitFeedbackComponent> {
	int framesElapsed = 0;
	int totalFrames = 36;
	int blinkInterval = 6;

	virtual void readFromJson(tson::TiledClass &j) override {};
};