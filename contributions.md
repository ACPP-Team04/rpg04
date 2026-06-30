# Contributions

## Marco Cotrotzo
- ECS `src/main/Abstract/ECS`
- Overworld `src/main/Abstract/Overworld` and `src/main/Implementation/Overworld`
Higlights:
- ECS `src/main/Abstract/ECS`
- `src/main/Implementation/Overworld/InteractionSystem`
- `src/main/Implementation/Overworld/CollisionSystem`
- `src/main/Implementation/Overworld/SwitchLayerSystem`
## Hannes Röd

## Jona Müller
- Combat `src/main/Implementation/Systems/CombatSystem.cpp`
- BattleInputSystem `src/main/Implementation/Systems/BattleInputSystem.cpp`
- Persistence & Save Game Management `src/main/Implementation/Persistance/PersistenceRegistrationSystem.cpp`
- Audio system

# Specification

## Goals (12 Points)

- [ ] **(2) Overworld with 2 different sections:**
  - Each with its own tone (architecture, sprites, music, enemies, …)
  - The second is only accessible after some story progress
  - Player can roam the world and interact with other entities
- [ ] **(1) Characters**
  - Player can talk to other characters
- [ ] **(1) Resources**
  - Player can manage acquired resources through dedicated menus
- [ ] **(1) Stats**
  - Player's combatants have stats that influence the combat
  - Player's combatants get experience from combat, increasing their stats
  - Stats are influenced by equipment
- [ ] **(3) Combat**
  - Turn-based
  - Player selects which attacks, spells, items, etc. to use on which target
  - Enemies use attacks, spells, items, etc. to combat the player
  - Game Over when all of player's combatants are dead
- [ ] **(1) Puzzle**
  - Introduce a small logic puzzle to progress in the game world
- [ ] **(1) Save points**
  - Player can save their progress at specific points in the game
  - Saved progress is persistent across play sessions (application termination)
- [ ] **(1) Audio**
  - Background music
  - Sound effects
- [ ] **(1) Main menu**
  - New game
  - Load game
  - Exit
