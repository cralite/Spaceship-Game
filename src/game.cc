#include "game.h"

#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>

#include <vector>
#include <iostream>
#include <math.h>
#include <chrono>
#include <cassert>
#include <random>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include "utils.h"

std::random_device g_rd;
std::mt19937 g_gen{ g_rd() };
std::uniform_real_distribution<float> g_asteroidAngleVelocity(0.05f, 0.5f);

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
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  m_window = SDL_CreateWindow("SpaceGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  m_context = SDL_GL_CreateContext(m_window);

  gladLoadGLLoader(SDL_GL_GetProcAddress);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
  ImGui_ImplOpenGL3_Init("#version 150");

  glViewport(0, 0, 1280, 720);

  m_shader.program = glCreateProgram();

  Utils::load_shader("data/shaders/shader.vert", ShaderType::eVertex, m_shader);
  Utils::load_shader("data/shaders/shader.frag", ShaderType::eFragment, m_shader);

  m_asteroidsTexture = Utils::load_texture("data/textures/asteroid.png");
  m_playerTexture = Utils::load_texture("data/textures/player.png");
  m_laserTexture = Utils::load_texture("data/textures/laser_beam.png");

  m_models[static_cast<size_t>(EntityType::AsteroidFragment)] = Utils::load_model("data/models/asteroid_fragment.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidSmall)] = Utils::load_model("data/models/asteroid_small.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidMedium)] = Utils::load_model("data/models/asteroid_medium.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidBig)] = Utils::load_model("data/models/asteroid_big.obj");
  m_models[static_cast<size_t>(EntityType::Player)] = Utils::load_model("data/models/player.obj");
  m_models[static_cast<size_t>(EntityType::LaserBeam)] = Utils::load_model("data/models/laser_beam.obj");

  setupCamera();

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
  glDebugMessageCallback(myGlDebugOutput, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}


void Game::setupCamera()
{
  m_camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
  m_camera.up = glm::vec3(0.0f, 0.0f, 1.0f);
  m_camera.lookAt = glm::vec3(0.0f, -1.0f, 0.0f);
  m_camera.offset = glm::vec3(0.0f, 50.0f, 18.0f);
  m_camera.direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

void Game::setupEntities()
{
  auto spawnEntity = [&](Model &a_model, Texture &a_texture) {
    auto entity = m_registry.create();
    m_registry.assign<Model>(entity, a_model);
    m_registry.assign<Texture>(entity, a_texture);
    m_registry.assign<Physics>(entity, Physics{});
    return entity;
  };

  m_player = spawnEntity(m_models[static_cast<size_t>(EntityType::Player)], m_playerTexture);

  auto &playerPhysics = m_registry.get<Physics>(m_player);
  playerPhysics.player = true;

  std::vector<glm::vec3> cubes = {
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f),
    glm::vec3( 6.0f,  1.0f,  3.0f)
  };

  std::uniform_int_distribution<size_t> randomAsteroidType(static_cast<size_t>(EntityType::AsteroidFragment), 
    static_cast<size_t>(EntityType::AsteroidBig));

  for (size_t i = 0; i < cubes.size(); ++i) {
    auto const modelIndex = randomAsteroidType(g_gen);
    auto const type = static_cast<EntityType>(modelIndex);

    Model model{ m_models[modelIndex] };
    Texture texture{ m_asteroidsTexture };

    auto asteroid = spawnEntity(model, texture);

    auto &physics = m_registry.get<Physics>(asteroid);
    std::uniform_real_distribution asteroidPos(-10.0f, 10.0f);
    physics.position = glm::vec3(asteroidPos(g_gen), 0.0f, asteroidPos(g_gen));
    physics.rotationAxis = glm::vec3(1.0f, 0.3f, 0.5f);
    physics.rotationVelocity = g_asteroidAngleVelocity(g_gen);
  }
}

void Game::loadSettings()
{
  using json = nlohmann::json;

  if (auto configData = Utils::open_file("data/config/config.json")) {
    json config{ json::parse((*configData).data()) };
    m_settings.cannonShootingFrequency = config["cannonShootingFrequency"].get<float>();
    m_settings.cannonShootingVelocity = config["cannonShootingVelocity"].get<float>();
    m_settings.spaceshipForwardVelocity = config["spaceshipForwardVelocity"].get<float>();
    m_settings.enginsThrust = config["enginsThrust"].get<float>();
    m_settings.spaceshipMass = config["spaceshipMass"].get<float>();
    m_settings.asteroidsAppearanceFrequency = config["asteroidsAppearanceFrequency"].get<float>();
    m_settings.asteroidsApperanceIncrease = config["asteroidsApperanceIncrease"].get<float>();
  } else {
      std::cerr << "cant load config";
  }
}

void Game::saveSettings()
{

}

void Game::handleWindowEvent(SDL_Event a_event)
{
  //switch (a_event) {

  //};
}

void Game::handleKeybordEvent(SDL_KeyboardEvent a_key, bool a_pressed)
{
  if (a_key.keysym.sym == SDLK_UP)
    m_keys[static_cast<size_t>(Key::eUp)] = a_pressed;
  else if (a_key.keysym.sym == SDLK_DOWN)
    m_keys[static_cast<size_t>(Key::eDown)] = a_pressed;
  else if (a_key.keysym.sym == SDLK_LEFT)
    m_keys[static_cast<size_t>(Key::eLeft)] = a_pressed;
  else if (a_key.keysym.sym == SDLK_RIGHT)
    m_keys[static_cast<size_t>(Key::eRight)] = a_pressed;
}

void Game::gameLoop()
{
  glEnable(GL_DEPTH_TEST);

  setupEntities();

  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double time{};

  glDepthMask(true);
  glUseProgram(m_shader.program);

  bool quit{};

  m_projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);

  float clearColor[3]{ 0.2f, 0.3f, 0.3f };

  while (!quit) {
    SDL_Event event{};

    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        //case SDL_WINDOWEVENT:
          //handleWindowEvent()
        case SDL_KEYUP:
          handleKeybordEvent(event.key, false);
          break;
        case SDL_KEYDOWN:
          handleKeybordEvent(event.key, true);
          break;
      };
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();

    debugDrawSystem();
    debugDrawEntitiesTree();

    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto now = clock_t::now();
    const duration deltaDuration = now - start;
    start = now;
    double delta{ deltaDuration.count() };
    time += delta;

    updateInput();

    updatePlayer(delta);
    updateEntities(delta);

    updateCamera();

    ImGui::Render();

    drawEntities();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
  }
}

void Game::updateInput()
{
  auto &physics = m_registry.get<Physics>(m_player);

  if (m_keys[static_cast<size_t>(Key::eLeft)])
    physics.position -= glm::normalize(glm::cross(m_camera.direction, m_camera.up)) * m_camera.speed * 0.016f;

  if (m_keys[static_cast<size_t>(Key::eRight)])
    physics.position += glm::normalize(glm::cross(m_camera.direction, m_camera.up)) * m_camera.speed * 0.016f;
}

void Game::updatePlayer(float a_delta)
{
  auto &model = m_registry.get<Model>(m_player);
  auto &texture = m_registry.get<Texture>(m_player);
  auto &physics = m_registry.get<Physics>(m_player);

//  physics.position = m_camera.pos + glm::vec3(0.0f, -10.0f, 15.0f);

  physics.modelMatrix = glm::mat4(1.0f);
  physics.modelMatrix = glm::translate(physics.modelMatrix, physics.position);
}

void Game::updateEntities(float a_delta)
{
  auto view = m_registry.view<Physics>();

  for (auto entity : view) {
    auto &physics = view.get<Physics>(entity);

    if (physics.player)
      continue;

    physics.position += physics.velocity * a_delta;
    physics.rotationAngle += physics.rotationVelocity * a_delta;
    
    if (physics.rotationAngle >= 360.f)
      physics.rotationAngle -= 360.f;

    physics.modelMatrix = glm::mat4(1.0f);
    physics.modelMatrix = glm::translate(physics.modelMatrix, physics.position);
    physics.modelMatrix = glm::rotate(physics.modelMatrix, glm::radians(physics.rotationAngle), physics.rotationAxis);
    physics.modelMatrix = glm::scale(physics.modelMatrix, glm::vec3(1.0f));
  }
}

void Game::updateCamera()
{
  auto& physics = m_registry.get<Physics>(m_player);
  m_camera.pos = physics.position + m_camera.offset;
  m_camera.pos.x = 0.0f;

  glm::mat4 view{ glm::lookAt(m_camera.pos, m_camera.pos + m_camera.lookAt, m_camera.up) };

  uint32_t const modelLocation = glGetUniformLocation(m_shader.program, "view");
  uint32_t const projectionLocation = glGetUniformLocation(m_shader.program, "projection");

  glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
}

void Game::drawEntities()
{
  auto view = m_registry.view<Texture, Model, Physics>();

  int modelLoc{ glGetUniformLocation(m_shader.program, "model") };

  for (auto entity : view) {
    auto &model = view.get<Model>(entity);
    auto &texture = view.get<Texture>(entity);
    auto &physics = view.get<Physics>(entity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.texture);
    glBindVertexArray(model.vao);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(physics.modelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, model.vertices);
  }
}

void Game::debugDrawSystem()
{
  ImGuiIO& io = ImGui::GetIO();

  ImGui::Begin("System");
  ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);
  ImGui::Text("FPS: %.3f ms", io.Framerate);
  ImGui::End();
}

void Game::debugDrawEntitiesTree()
{
  ImGui::Begin("Entities");

  auto view = m_registry.view<Physics>();

  size_t const count = view.size();

  for (size_t i = 0; i < count; ++i) {
    auto entity = view[i];
    auto& physics = view.get<Physics>(entity);

    if (ImGui::TreeNode((void*)(intptr_t)i, "Entity #%02d%s", i, physics.player ? " (player)" : ""))
    {
      ImGui::InputFloat3("position", glm::value_ptr(physics.position));
      ImGui::InputFloat3("velocity", glm::value_ptr(physics.velocity));
      ImGui::InputFloat3("rotationAxis", glm::value_ptr(physics.rotationAxis));
      ImGui::InputFloat("rotationAngle", &physics.rotationAngle);
      ImGui::InputFloat("rotationVelocity", &physics.rotationVelocity);
      ImGui::TreePop();
    }

  }

  ImGui::End();

  ImGui::Begin("Camera");
  ImGui::InputFloat3("up", glm::value_ptr(m_camera.up));
  ImGui::InputFloat3("position", glm::value_ptr(m_camera.pos));
  ImGui::InputFloat3("offset", glm::value_ptr(m_camera.offset));
  ImGui::InputFloat3("lookAt", glm::value_ptr(m_camera.lookAt));
  ImGui::End();
}
