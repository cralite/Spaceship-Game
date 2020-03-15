#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <glad/glad.h>
#include <string>
#include <array>
#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include "utils.h"

class Game {
public:
  Game();
  ~Game() = default;

  void setupCamera();
  void setupPlayer();

  entt::entity spawnEntity(Model& a_model, Texture& a_texture);
  void spawnAsteroids();
  void spawnAsteroid();

  void loadSettings();
  void saveSettings();

  void handleWindowEvent(SDL_Event a_event);
  void handleKeybordEvent(SDL_KeyboardEvent a_key, bool a_pressed);
  bool isAsteroid(EntityType a_type);
  bool hasCollision(Physics const& entity1, Physics const& entity2);

  void gameLoop();

  std::string_view getEntityTypeName(EntityType a_type);

  void updateInput(float a_delta);
  void updatePlayer(float a_delta);
  void updateEntities(float a_delta);
  void updateCamera();
  void drawEntities();
  void drawPoints();

  void shoot();
  void checkCollision();

  void debugDrawSystem();
  void debugDrawEntitiesTree();
  void debugDrawParams();

private:
  SDL_Window* m_window{};
  SDL_GLContext m_context{};
  Shader m_shader{};
  Camera m_camera{};

  Texture m_asteroidsTexture;
  Texture m_playerTexture;
  Texture m_laserTexture;
  std::array<Model, static_cast<size_t>(EntityType::Count)> m_models{};
  std::array<float, static_cast<size_t>(EntityType::Count)> m_scales{};
  std::array<float, static_cast<size_t>(EntityType::Count)> m_radiuses{};
  std::array<int32_t, static_cast<size_t>(EntityType::Count)> m_pointsPerAsteroid{};

  entt::registry m_registry{};
  entt::entity m_player{};

  glm::mat4 m_projectionMatrix{};

  std::array<bool, static_cast<size_t>(Key::Count)> m_keys{};
  std::vector<std::pair<entt::entity, entt::entity>> m_collided{};
  Settings m_settings{};
  uint32_t m_points{};
  bool m_shoot{};
  bool m_drawDebugBoxes{};

  float m_asteroidsAppearanceFrequency{};
};

#endif // GAME_H