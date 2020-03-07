#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <glad/glad.h>
#include <optional>
#include <string>

class Game {
 public:
  Game();
  ~Game();

  void gameLoop();
  std::optional<std::string> openFile(std::string a_path);
  void loadShaders();
  void loadTriangle();
  void loadTexture(const std::string a_path, int a_idx);

  void move(float a_angle);

 private:
  SDL_Window *m_window{};
  SDL_GLContext m_context{};
  unsigned int m_shaderProgram{};
  unsigned int m_vao{};
  unsigned int m_texture[2]{};
};

#endif // GAME_H