#include "game.h"

#include <GL/GL.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <chrono>

const int VERTEX_LOCATION = 0;
const int COLOR_LOCATION = 1;
const int TEXTURE_LOCATION = 2;

Game::Game()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

  m_window = SDL_CreateWindow("SpaceGame", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
  m_context = SDL_GL_CreateContext(m_window);

  gladLoadGLLoader(SDL_GL_GetProcAddress);
  glViewport(0, 0, 1024, 768);
  loadShaders();
}

Game::~Game()
{
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Game::gameLoop()
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  loadTriangle();

  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double time{};

  while (true) {
    SDL_Event event{};

    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        break;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    const auto now = clock_t::now();
    const duration delta = now - start;
    start = now;

    //time += delta.count() / 1000.0;

    //float greenValue{ static_cast<float>(std::sin(time)) / 2.0f + 0.5f };
    //int vertexColorLocation{ glGetUniformLocation(m_shaderProgram, "ourColor") };

    glBindVertexArray(m_vao);
    glUseProgram(m_shaderProgram);
    //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_window);
  }
}

std::optional<std::string> Game::openFile(std::string a_path)
{
  std::fstream fs{};
  std::string output{};
  fs.open(a_path, std::ios::in);
  std::cerr << a_path << std::endl;
  if (fs.is_open()) {
    while (fs.good()) {
      std::string str{};
      std::getline(fs, str);
      output += str + "\n";
    }
    fs.close();
  } else {
    std::cerr << "cannot open file" << std::endl;
    return {};
  }
  return output;
}

void Game::loadShaders()
{
  auto compileShader = [](std::string& a_shaderSource, bool a_isVertex) {
    unsigned int shader{ a_isVertex ? glCreateShader(GL_VERTEX_SHADER) : glCreateShader(GL_FRAGMENT_SHADER) };
    const char *const shaderData = a_shaderSource.data();
    glShaderSource(shader, 1, &shaderData, nullptr);
    glCompileShader(shader);
    return shader;
  };

  m_shaderProgram = glCreateProgram();

  if (auto vertexShaderSource = openFile("D:/Develop/SpaceshipGame/data/shaders/vertexShader.vert")) {
    auto vertexShader = compileShader(*vertexShaderSource, true);
    int dupa{};
    char log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &dupa);
    glGetShaderInfoLog(vertexShader, 512, nullptr, log);
    std::cerr << log << std::endl;
    
    glAttachShader(m_shaderProgram, vertexShader);  
  }

  if (auto fragmentShaderSource = openFile("D:/Develop/SpaceshipGame/data/shaders/fragmentShader.frag")) {
    auto fragmentShader = compileShader(*fragmentShaderSource, false);

    glAttachShader(m_shaderProgram, fragmentShader);
  }

  glLinkProgram(m_shaderProgram);
  //glDeleteShader(vertexShader);
  //glDeleteShader(fragmentShader);
}

void Game::loadTriangle()
{
  std::vector<float> vertices = {
     0.5f, -0.5f, 0.0f,
     1.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,
     0.0f,  0.0f, 1.0f
  };

  unsigned int vbo{};

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VERTEX_LOCATION);

  glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(12));
  glEnableVertexAttribArray(COLOR_LOCATION);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //glBindBuffer(GL_ARRAY_BUFFER, COLOR_LOCATION);
  //glBindVertexArray(COLOR_LOCATION);
}
