#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <glad/glad.h>
#include <string>
#include <entt/entt.hpp>

#include "utils.h"

class Game {
 public:
  Game();
  ~Game() = default;

  void setupMatrix();
  void handleWindowEvent(SDL_Event a_event);

  void gameLoop();
 
  void loadTriangle();
  void drawCubes();

  void move(float a_angle);

 private:
  SDL_Window *m_window{};
  SDL_GLContext m_context{};
  Shader m_shader{};
  entt::registry m_registry{};
};

#endif // GAME_H