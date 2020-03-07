#include "game.h"

#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int VERTEX_LOCATION = 0;
const int TEXTURE_LOCATION = 1;
const int COLOR_LOCATION = 2;

Game::Game()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  constexpr int GAME_GL_DEBUG_CONTEXT_FLAGS = SDL_GL_CONTEXT_DEBUG_FLAG;
//  constexpr int GAME_GL_DEBUG_CONTEXT_FLAGS = 0;

  constexpr int GAME_GL_CONTEXT_FLAGS = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | GAME_GL_DEBUG_CONTEXT_FLAGS;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, GAME_GL_CONTEXT_FLAGS);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  m_window = SDL_CreateWindow("SpaceGame", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
  m_context = SDL_GL_CreateContext(m_window);

  gladLoadGLLoader(SDL_GL_GetProcAddress);
  glViewport(0, 0, 1024, 768);
  loadShaders();
}

Game::~Game()
{
  SDL_Quit();
}

void Game::setupMatrix()
{
  glm::mat4 model{ glm::mat4(1.0f) };
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 view{ glm::mat4(1.0f) };
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 1024.0f/768.0f, 0.1f, 100.0f) };

  int modelLoc{ glGetUniformLocation(m_shaderProgram, "model") };
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  modelLoc = glGetUniformLocation(m_shaderProgram, "view");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));

  modelLoc = glGetUniformLocation(m_shaderProgram, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
void Game::gameLoop()
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  loadTriangle();
  loadTexture("D:/Develop/SpaceshipGame/data/wall.jpg", 0);
  loadTexture("D:/Develop/SpaceshipGame/data/awesomeface.png", 1);

  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double time{};

  glUseProgram(m_shaderProgram);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "texture2"), 1);

  while (true) {
    SDL_Event event{};

    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        break;
    }

    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto now = clock_t::now();
    const duration delta = now - start;
    start = now;

    glUseProgram(m_shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture[0]); 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture[1]);

    setupMatrix();

    glBindVertexArray(m_vao);

    move(glm::radians(time += (delta.count() / 10.0f)));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    drawCubes();

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

    int dupa{};
    char log[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &dupa);
    glGetShaderInfoLog(fragmentShader, 512, nullptr, log);
    std::cerr << log << std::endl;

    glAttachShader(m_shaderProgram, fragmentShader);
  }

  glLinkProgram(m_shaderProgram);
  //glDeleteShader(vertexShader);
  //glDeleteShader(fragmentShader);
}

void Game::loadTriangle()
{
  std::vector<float> vertices = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };

  unsigned int vbo{};

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VERTEX_LOCATION);

  glVertexAttribPointer(TEXTURE_LOCATION, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(12));
  glEnableVertexAttribArray(TEXTURE_LOCATION);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Game::loadTexture(const std::string a_path, int a_idx)
{
  glGenTextures(1, &m_texture[a_idx]);
  glBindTexture(GL_TEXTURE_2D, m_texture[a_idx]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);

  int width{}, height{}, nrChannels{};
  unsigned char* data = stbi_load(a_path.c_str(), &width, &height, &nrChannels, 0);
  if (!data) {
    std::cerr << "cannot load texture" << std::endl;
    return;
  }

  const auto format = nrChannels == 4 ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
}

void Game::drawCubes()
{
  std::vector<glm::vec3> cubes = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };


  for (size_t i = 0; i < cubes.size(); ++i) {
    glm::mat4 model{ glm::mat4(1.0f) };
    model = glm::translate(model, cubes[i]);
    float angle{ 20.0f * i };
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.05f));
    int modelLoc{ glGetUniformLocation(m_shaderProgram, "model") };
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

void Game::move(float a_angle)
{
  glm::mat4 trans{ glm::mat4(1.0f) };
  trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 0.5f));
  trans = glm::rotate(trans, a_angle, glm::vec3(1.0f, 0.0f, 1.0f));
  unsigned int transformLoc= glGetUniformLocation(m_shaderProgram, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
}