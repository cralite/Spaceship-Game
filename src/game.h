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

  entt::entity spawnEntity(Model &a_model, Texture &a_texture);
  void spawnAsteroids();
  void spawnAsteroid();

  void loadSettings();
  void saveSettings();

  void handleWindowEvent(SDL_Event a_event);
  void handleKeybordEvent(SDL_KeyboardEvent a_key, bool a_pressed);

  void gameLoop();
 
  void updateInput(float a_delta);
  void updatePlayer(float a_delta);
  void updateEntities(float a_delta);
  void updateCamera();
  void drawEntities();

  void debugDrawSystem();
  void debugDrawEntitiesTree();

 private:
  enum class EntityType {
    AsteroidFragment,
    AsteroidSmall,
    AsteroidMedium,
    AsteroidBig,
    Player,
    LaserBeam,
    Count
  };

  SDL_Window *m_window{};
  SDL_GLContext m_context{};
  Shader m_shader{};
  Camera m_camera{};

  Texture m_asteroidsTexture;
  Texture m_playerTexture;
  Texture m_laserTexture;
  std::array<Model, static_cast<size_t>(EntityType::Count)> m_models;
  
  entt::registry m_registry{};
  entt::entity m_player{};

  glm::mat4 m_projectionMatrix{};
  
  std::array<bool, static_cast<size_t>(Key::eCount)> m_keys{};
  Settings m_settings{};

  float m_asteroidsAppearanceFrequency{};
};

#endif // GAME_H