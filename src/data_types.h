#ifndef DATA_TYPES_H
#define DATA_TYPES_H

enum class GameState {
  Playing,
  EndGame
};

struct Settings {
  float cannonShootingFrequency{};
  float cannonShootingVelocity{};
  float spaceshipForwardVelocity{};
  float asteroidsAngularVelocityRange{};
  float engineThrust{};
  float spaceshipMass{};
  float asteroidsAppearanceFrequency{};
  float asteroidsApperanceIncrease{};
};

#endif //DATA_TYPES_H