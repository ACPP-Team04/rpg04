class CombatSystem {
  public:
	// FIXME: Add later
	// Check if there are entities in battleState if yes, deal with their plays
	void update(float deltaTime);

	// add the type of attack as parameter
	void executeAttack(Entity &attacker, Entity &defender, int typeOfAttack);

	void takeHealAction(Entity &healer);

	void restoreAP(Entity &restorator);

  private:
	float getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent)
	{
		if (weaponComponent->weaponType == WeaponType::MELEE) {
			return weaponComponent->baseDamage + statsComponent.strength * this->getMultiplicatorFromScalingFactor();
		}
		if (weaponComponent->weaponType == WeaponType::RANGE) {
			return weaponComponent->baseDamage + statsComponent.dexterity * this->getMultiplicatorFromScalingFactor();
		}
	}

	float getMultiplicatorFromScalingFactor(const WeaponComponent &weaponComponent)
	{
		switch (weaponComponent->scalingFactor) {

		case ScalingFactor::A:
			return 3.0f;

		case ScalingFactor::B:
			return 2.0f;

		case ScalingFactor::C:
			return 1.0f;
		}
	}
};
