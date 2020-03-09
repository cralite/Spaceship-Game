#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <glad/glad.h>
#include <string>
#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include "utils.h"

class Game {
 public:
  Game();
  ~Game() = default;

  void setupMatrix(glm::mat4 a_view);
  void setupCamera();
  void handleWindowEvent(SDL_Event a_event);
  void handleKeybordEvent(SDL_KeyboardEvent a_key);

  void gameLoop();
 
  void loadTriangle();
  void drawCubes(float a_angle);

  void move(float a_angle);

 private:
  SDL_Window *m_window{};
  SDL_GLContext m_context{};
  Shader m_shader{};
  Camera m_camera{};
  entt::registry m_registry{};
};

#endif // GAME_H