#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/System/System.hpp"
class PersistenceRegistrationSystem : public System {
  public:
	explicit PersistenceRegistrationSystem(ArchetypeManager &manager);
	void update() override;
};