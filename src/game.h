#ifndef GAME_H
#define GAME_H

#include <string>
#include <array>

#include "engine/engine.h"
#include <entt/entt.hpp>
#include "data_types.h"

class Game {
public:
  Game();
  ~Game() = default;

  void setupPlayer();

    
  void spawnAsteroids();
  void spawnAsteroid();

  void loadSettings();
  void saveSettings();

  void checkCollision();
  bool isAsteroid(engineDataType::EntityType a_type);

  void gameLoop();

  std::string_view getengineDataType(engineDataType::EntityType a_type);

  void updateInput(float a_delta);
  
  void drawPoints();
  void drawEndGame();

  void shoot();

  void reset();

  void debugDrawSystem();
  void debugDrawEntitiesTree();
  void debugDrawParams();


private:
  Engine m_engine{};
  std::array<int32_t, static_cast<size_t>(engineDataType::EntityType::Count)> m_pointsPerAsteroid{};

  entt::entity m_player{};

  std::array<float, static_cast<size_t>(engineDataType::EntityType::Count)> m_scales{};
  std::array<float, static_cast<size_t>(engineDataType::EntityType::Count)> m_radiuses{};

  Settings m_settings{};
  uint32_t m_points{};
  GameState m_gameState{};
  bool m_shoot{};
  bool m_drawDebugBoxes{};

  float m_asteroidsAppearanceFrequency{};
};

#endif // GAME_H