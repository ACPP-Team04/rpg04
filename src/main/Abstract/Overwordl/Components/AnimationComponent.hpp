#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "SpriteComponent.hpp"

#include <deque>
#include <nlohmann/json.hpp>

struct Animation {
	int entityAnimationSpriteId;
	int numFrames;
};

class AnimationSequence {
	std::deque<Animation> sequence;

  public:
	void push_back(Animation animation) { sequence.push_back(animation); }
	std::tuple<Animation, bool> getCurrentAnimation(const int framesElapsed)
	{
		bool resetFrame = false;
		const Animation currentAnimation = sequence.front();
		if (framesElapsed >= currentAnimation.numFrames) {
			sequence.pop_front();
			sequence.push_back(currentAnimation);
			resetFrame = true;
		}
		return std::make_tuple(currentAnimation, resetFrame);
	}
};

class AnimationComponent : public Component<AnimationComponent> {
  private:
	std::unordered_map<ENTITY_ANIMATIONS_STATE, AnimationSequence> animations;
	std::optional<int> currentAnimationEntityId = 0;
	int framesElapsed{};

  public:
	void readFromJson(tson::TiledClass &j) override
	{
		tson::TiledClass sequences = j.get<tson::TiledClass>("sequences");
		int i = 0;
		while (sequences.getMember(std::to_string(i)) != nullptr) {
			tson::TiledClass seq = sequences.get<tson::TiledClass>(std::to_string(i));
			ENTITY_ANIMATIONS_STATE state = ENTITY_ANIMATIONS_STATE(seq.get<int>("state"));

			AnimationSequence animSeq;
			tson::TiledClass anims = seq.get<tson::TiledClass>("sequence");
			int k = 0;
			while (anims.getMember(std::to_string(k)) != nullptr) {
				tson::TiledClass anim = anims.get<tson::TiledClass>(std::to_string(k));
				Animation animation = {
					anim.get<int>("entitySpriteId"),
					anim.get<int>("numFrames")
				};
				animSeq.push_back(animation);
				k++;
			}
			addAnimation(state, animSeq);
			i++;
		}
		int id = 0;
	}
	void setCurrentAnimation(ENTITY_ANIMATIONS_STATE state)
	{
		if (!this->animations.contains(state)) {
			this->currentAnimationEntityId = std::nullopt;
			return;
		}
		AnimationSequence &sequence = animations.at(state);
		auto [anim, reset] = sequence.getCurrentAnimation(framesElapsed);

		if (reset) {
			framesElapsed = 0;
		} else {
			framesElapsed++;
		}
		currentAnimationEntityId = anim.entityAnimationSpriteId;
	}

	std::optional<int> getCurrentAnimation() const { return currentAnimationEntityId; }
	void addAnimation(ENTITY_ANIMATIONS_STATE state, const AnimationSequence &animation_sequence)
	{
		this->animations[state] = animation_sequence;
	}
};