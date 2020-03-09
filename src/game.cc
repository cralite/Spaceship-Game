#include "game.h"
#include "utils.h"

#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <math.h>
#include <chrono>
#include <cassert>

Model model{};
Texture texture{};  

//void APIENTRY myGlDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void APIENTRY myGlDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  // ignore non-significant error/warning codes
  if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " <<  message << std::endl;

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
  case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
  case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
  }
  std::cout << std::endl;

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
  case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
  case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
  case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
  }
  std::cout << std::endl;

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
  case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
  case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
  }
  std::cout << std::endl;
  assert(false);
}

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
  m_shader.m_program = glCreateProgram();
  Utils::load_shader("D:/Develop/SpaceshipGame/data/shaders/vertexShader.vert", ShaderType::eVertex, m_shader);
  Utils::load_shader("D:/Develop/SpaceshipGame/data/shaders/fragmentShader.frag", ShaderType::eFragment, m_shader);
  setupCamera();

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
  glDebugMessageCallback(myGlDebugOutput, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}


void Game::setupMatrix(glm::mat4 a_view)
{
  glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 1024.0f/768.0f, 0.1f, 100.0f) };

  uint32_t modelLoc = glGetUniformLocation(m_shader.m_program, "view");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(a_view));

  modelLoc = glGetUniformLocation(m_shader.m_program, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Game::setupCamera()
{
  m_camera.m_pos = glm::vec3(0.0f, 0.0f, 3.0f);
  m_camera.m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
  m_camera.m_right = glm::normalize(glm::cross(m_camera.m_up, m_camera.m_direction));
  //m_camera.m_up = glm::cross(m_camera.m_direction, m_camera.m_right);
  m_camera.m_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Game::handleWindowEvent(SDL_Event a_event)
{
  //switch (a_event) {

  //};
}

void Game::handleKeybordEvent(SDL_KeyboardEvent a_key)
{
  switch (a_key.keysym.sym) {
    case SDLK_UP: {
      m_camera.m_pos += m_camera.m_speed * m_camera.m_direction;
      break;
    }
    case SDLK_DOWN: {
      m_camera.m_pos -= m_camera.m_speed * m_camera.m_direction;
      break;
    }
    case SDLK_LEFT: {
      m_camera.m_pos -= glm::normalize(glm::cross(m_camera.m_direction, m_camera.m_up)) * m_camera.m_speed;
      break;
    }
    case SDLK_RIGHT: {
      m_camera.m_pos += glm::normalize(glm::cross(m_camera.m_direction, m_camera.m_up)) * m_camera.m_speed;
      break;
    }
  }
}

void Game::gameLoop()
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  loadTriangle();
  Texture texture1 = Utils::load_texture("D:/Develop/SpaceshipGame/data/wall.jpg");
  Texture texture2 = Utils::load_texture("D:/Develop/SpaceshipGame/data/awesomeface.png");

  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double time{};

  glUseProgram(m_shader.m_program);
  glUniform1i(glGetUniformLocation(m_shader.m_program, "texture1"), 0);
  glUniform1i(glGetUniformLocation(m_shader.m_program, "texture2"), 1);

  while (true) {
    SDL_Event event{};

    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          break;
        //case SDL_WINDOWEVENT:
          //handleWindowEvent()
        case SDL_KEYDOWN:
          handleKeybordEvent(event.key);
          break;
      };
      if (event.type == SDL_QUIT)
        break;
    }

    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto now = clock_t::now();
    const duration deltaDuration = now - start;
    start = now;
    double delta{ deltaDuration.count() };
    time += delta;

    glUseProgram(m_shader.m_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1.m_texture); 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2.m_texture);

    glm::mat4 view{ glm::lookAt(m_camera.m_pos, m_camera.m_pos + m_camera.m_direction, m_camera.m_up) };
    setupMatrix(view);

    glBindVertexArray(model.m_vao);

    //player
    //model dla playera
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    drawCubes(glm::radians(time));

    SDL_GL_SwapWindow(m_window);
  }
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

  model = Utils::load_model(vertices);
}

void Game::drawCubes(float a_angle)
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
    glm::mat4 cube{ glm::mat4(1.0f) };
    cube = glm::translate(cube, cubes[i]);
    float angle{ 20.0f * i + a_angle };
    cube = glm::rotate(cube, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    int modelLoc{ glGetUniformLocation(m_shader.m_program, "model") };
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cube));
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

void Game::move(float a_angle)
{
  glm::mat4 trans{ glm::mat4(1.0f) };
  trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 0.5f));
  trans = glm::rotate(trans, a_angle, glm::vec3(1.0f, 0.0f, 1.0f));
  unsigned int transformLoc= glGetUniformLocation(m_shader.m_program, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
}
