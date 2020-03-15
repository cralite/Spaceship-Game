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
std::uniform_real_distribution<float> g_asteroidAngleVelocity(10.05f, 30.5f);

std::vector<float> g_vertices = {
  -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
   1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
  -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

  -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
   1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
  -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

  -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
  -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
  -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

   1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
   1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
   1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
   1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
   1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

  -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
   1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
   1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
   1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
  -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
  -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

  -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
   1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
   1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
  -1.0f,  1.0f, -1.0f,  0.0f, 1.0f
};

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

  Utils::load_shader("data/shaders/shader.vert", ShaderType::Vertex, m_shader);
  Utils::load_shader("data/shaders/shader.frag", ShaderType::Fragment, m_shader);

  m_asteroidsTexture = Utils::load_texture("data/textures/asteroid.png");
  m_playerTexture = Utils::load_texture("data/textures/player.png");
  m_laserTexture = Utils::load_texture("data/textures/laser_beam.png");

  m_models[static_cast<size_t>(EntityType::AsteroidFragment)] = Utils::load_model("data/models/asteroid_fragment.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidSmall)] = Utils::load_model("data/models/asteroid_small.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidMedium)] = Utils::load_model("data/models/asteroid_medium.obj");
  m_models[static_cast<size_t>(EntityType::AsteroidBig)] = Utils::load_model("data/models/asteroid_big.obj");
  m_models[static_cast<size_t>(EntityType::LaserBeam)] = Utils::load_model("data/models/laser_beam.obj");
  m_models[static_cast<size_t>(EntityType::Player)] = Utils::load_model("data/models/player.obj");
  m_models[static_cast<size_t>(EntityType::Box)] = Utils::load_model(g_vertices);

  loadSettings();
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
  m_camera.direction = glm::vec3(0.0f, 0.0f, 1.0f);
  m_camera.lookAt = glm::vec3(0.0f, -1.0f, 0.0f);
  m_camera.offset = glm::vec3(0.0f, 50.0f, 18.0f);
}

void Game::setupPlayer()
{
  m_player = spawnEntity(m_models[static_cast<size_t>(EntityType::Player)], m_playerTexture);

  auto &physics = m_registry.get<Physics>(m_player);
  physics.entityType = EntityType::Player;
}

entt::entity Game::spawnEntity(Model& a_model, Texture& a_texture)
{
  auto entity = m_registry.create();
  m_registry.assign<Model>(entity, a_model);
  m_registry.assign<Texture>(entity, a_texture);
  m_registry.assign<Physics>(entity, Physics{});
  return entity;
}

void Game::spawnAsteroids()
{
  //for (size_t i = 0; i < m_asteroidsAppearanceFrequency; ++i)
    spawnAsteroid();
}

void Game::spawnAsteroid()
{
  std::uniform_int_distribution<size_t> randomAsteroidType(static_cast<size_t>(EntityType::AsteroidFragment), 
    static_cast<size_t>(EntityType::AsteroidBig));

  auto const modelIndex = randomAsteroidType(g_gen);
  auto const type = static_cast<EntityType>(modelIndex);

  auto asteroid = spawnEntity(m_models[modelIndex], m_asteroidsTexture);

  auto &playerPhysics = m_registry.get<Physics>(m_player);
  auto &playerPos = playerPhysics.position;

  auto &physics = m_registry.get<Physics>(asteroid);
  physics.entityType = type;

  constexpr float distanceMinX = -20.0f;
  constexpr float distanceMaxX = 20.0f;

  constexpr float distanceMinZ = 40.0f;
  constexpr float distanceMaxZ = 70.0f;

  std::uniform_real_distribution asteroidPosX(playerPos.x + distanceMinX, playerPos.x + distanceMaxX);
  std::uniform_real_distribution asteroidPosZ(playerPos.z + distanceMinZ, playerPos.z + distanceMaxZ);
  std::uniform_real_distribution rotationAxis(-1.0f, 1.0f);

  physics.position = glm::vec3(asteroidPosX(g_gen), 0.0f, asteroidPosZ(g_gen));
  physics.rotationAxis = glm::vec3(rotationAxis(g_gen), rotationAxis(g_gen), rotationAxis(g_gen));
  physics.rotationVelocity = g_asteroidAngleVelocity(g_gen);
}

void Game::loadSettings()
{
  using json = nlohmann::json;

  if (auto configData = Utils::open_file("data/configs/config.json")) {
    json config{ json::parse((*configData).data()) };
    m_settings.cannonShootingFrequency = config["cannonShootingFrequency"].get<float>();
    m_settings.cannonShootingVelocity = config["cannonShootingVelocity"].get<float>();
    m_settings.spaceshipForwardVelocity = config["spaceshipForwardVelocity"].get<float>();
    m_settings.engineThrust = config["engineThrust"].get<float>();
    m_settings.spaceshipMass = config["spaceshipMass"].get<float>();
    m_settings.asteroidsAppearanceFrequency = config["asteroidsAppearanceFrequency"].get<float>();
    m_settings.asteroidsApperanceIncrease = config["asteroidsApperanceIncrease"].get<float>();

    auto scales = config["scales"];
    m_scales[static_cast<size_t>(EntityType::AsteroidFragment)] = scales["AsteroidFragment"].get<float>();
    m_scales[static_cast<size_t>(EntityType::AsteroidSmall)] = scales["AsteroidSmall"].get<float>();
    m_scales[static_cast<size_t>(EntityType::AsteroidMedium)] = scales["AsteroidMedium"].get<float>();
    m_scales[static_cast<size_t>(EntityType::AsteroidBig)] = scales["AsteroidBig"].get<float>();
    m_scales[static_cast<size_t>(EntityType::LaserBeam)] = scales["LaserBeam"].get<float>();
    m_scales[static_cast<size_t>(EntityType::Player)] = scales["Player"].get<float>();

    auto radius = config["radius"];
    m_radiuses[static_cast<size_t>(EntityType::AsteroidFragment)] = radius["AsteroidFragment"].get<float>();
    m_radiuses[static_cast<size_t>(EntityType::AsteroidMedium)] = radius["AsteroidMedium"].get<float>();
    m_radiuses[static_cast<size_t>(EntityType::AsteroidSmall)] = radius["AsteroidSmall"].get<float>();
    m_radiuses[static_cast<size_t>(EntityType::AsteroidBig)] = radius["AsteroidBig"].get<float>();
    m_radiuses[static_cast<size_t>(EntityType::LaserBeam)] = radius["LaserBeam"].get<float>();
    m_radiuses[static_cast<size_t>(EntityType::Player)] = scales["Player"].get<float>();


    auto points = config["points"];
    m_pointsPerAsteroid[static_cast<size_t>(EntityType::AsteroidFragment)] = points["AsteroidFragment"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(EntityType::AsteroidMedium)] = points["AsteroidMedium"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(EntityType::AsteroidSmall)] = points["AsteroidSmall"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(EntityType::AsteroidBig)] = points["AsteroidBig"].get<int32_t>();
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
  if (a_key.keysym.sym == SDLK_SPACE)
    m_keys[static_cast<size_t>(Key::Space)] = a_pressed;
  else if (a_key.keysym.sym == SDLK_LEFT)
    m_keys[static_cast<size_t>(Key::Left)] = a_pressed;
  else if (a_key.keysym.sym == SDLK_RIGHT)
    m_keys[static_cast<size_t>(Key::Right)] = a_pressed;
}

bool Game::isAsteroid(EntityType a_type)
{
  return a_type == EntityType::AsteroidBig || a_type == EntityType::AsteroidFragment ||
    a_type == EntityType::AsteroidMedium || a_type == EntityType::AsteroidSmall;
}

bool Game::hasCollision(Physics const& entity1, Physics const& entity2)
{
  auto const type1 = static_cast<size_t>(entity1.entityType);
  auto const type2 = static_cast<size_t>(entity2.entityType);

  auto length = glm::length(entity1.position - entity2.position);
  int l = static_cast<int>(length);
  return length <= (m_radiuses[type1] + m_radiuses[type2]);
}

void Game::gameLoop()
{
  glEnable(GL_DEPTH_TEST);

  m_asteroidsAppearanceFrequency = m_settings.asteroidsAppearanceFrequency;

  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double asteroidSpawnTime{};
  double lasersSpawnTime{};

  double laserTimeDiff = 1.0 / m_settings.cannonShootingFrequency;

  glDepthMask(true);
  glUseProgram(m_shader.program);

  bool quit{};

  m_projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);

  float clearColor[3]{ 0.2f, 0.3f, 0.3f };

  setupPlayer();
  spawnAsteroids();

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

    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto now = clock_t::now();
    const duration deltaDuration = now - start;
    start = now;
    double delta{ deltaDuration.count() / 1000.0 };

    asteroidSpawnTime += delta;

    if (asteroidSpawnTime >= 1.0f) {
      spawnAsteroids();
      asteroidSpawnTime = 0.0f;
    }

    updateInput(delta);

    if (m_shoot) {
      if (lasersSpawnTime >= laserTimeDiff)
        lasersSpawnTime = 0.0f;

      if (lasersSpawnTime == 0.0)
        shoot();

      lasersSpawnTime += delta;
    }
    else
      lasersSpawnTime = 0.0;

    updatePlayer(delta);
    updateEntities(delta);
    checkCollision();

    updateCamera();

    drawEntities();

    drawPoints();

    debugDrawSystem();
    debugDrawEntitiesTree();
    debugDrawParams();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
  }

  saveSettings();
}

std::string_view Game::getEntityTypeName(EntityType a_type)
{
  switch (a_type)
  {
    case EntityType::AsteroidFragment: return "AsteroidFragment";
    case EntityType::AsteroidSmall: return "AsteroidSmall";
    case EntityType::AsteroidMedium: return "AsteroidMedium";
    case EntityType::AsteroidBig: return "AsteroidBig";
    case EntityType::Player: return "Player";
    case EntityType::LaserBeam: return "LaserBeam";
  }

  return "<unknown>";
}

void Game::updateInput(float a_delta)
{
  auto &physics = m_registry.get<Physics>(m_player);

  glm::vec3 const direction{ -1.0f, 0.0f, 0.0f };

  if (m_keys[static_cast<size_t>(Key::Left)])
    physics.position -= direction * m_camera.speed * a_delta;

  if (m_keys[static_cast<size_t>(Key::Right)])
    physics.position += direction * m_camera.speed * a_delta;

  m_shoot = m_keys[static_cast<size_t>(Key::Space)];
}

void Game::updatePlayer(float a_delta)
{
  auto &model = m_registry.get<Model>(m_player);
  auto &texture = m_registry.get<Texture>(m_player);
  auto &physics = m_registry.get<Physics>(m_player);

  physics.position += m_settings.spaceshipForwardVelocity * m_camera.direction * a_delta;

  physics.modelMatrix = glm::mat4(1.0f);
  physics.modelMatrix = glm::translate(physics.modelMatrix, physics.position);

  physics.debugBoxMatrix = glm::mat4(1.0f);
  physics.debugBoxMatrix = glm::translate(physics.debugBoxMatrix, physics.position);
  physics.debugBoxMatrix = glm::scale(physics.debugBoxMatrix, glm::vec3(m_radiuses[static_cast<size_t>(physics.entityType)]));
}

void Game::updateEntities(float a_delta)
{
  auto view = m_registry.view<Physics>();

  for (auto entity : view) {
    auto &physics = view.get<Physics>(entity);

    if (physics.entityType == EntityType::Player)
      continue;

    physics.velocity += physics.acceleration * a_delta;
    physics.position += physics.velocity * a_delta;
    physics.rotationAngle += physics.rotationVelocity * a_delta;
    
    if (physics.rotationAngle >= 360.f)
      physics.rotationAngle -= 360.f;

    physics.modelMatrix = glm::mat4(1.0f);
    physics.modelMatrix = glm::translate(physics.modelMatrix, physics.position);
    physics.modelMatrix = glm::rotate(physics.modelMatrix, glm::radians(physics.rotationAngle), physics.rotationAxis);
    physics.modelMatrix = glm::scale(physics.modelMatrix, glm::vec3(m_scales[static_cast<size_t>(physics.entityType)]));

    physics.debugBoxMatrix = glm::mat4(1.0f);
    physics.debugBoxMatrix = glm::translate(physics.debugBoxMatrix, physics.position);
    physics.debugBoxMatrix = glm::scale(physics.debugBoxMatrix, glm::vec3(m_radiuses[static_cast<size_t>(physics.entityType)]));
  }
}

void Game::updateCamera()
{
  auto &physics = m_registry.get<Physics>(m_player);
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

    if (m_drawDebugBoxes) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      auto& boxModel = m_models[static_cast<size_t>(EntityType::Box)];

      glBindVertexArray(boxModel.vao);

      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(physics.debugBoxMatrix));

      glDrawArrays(GL_TRIANGLES, 0, model.vertices);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }
}

void Game::drawPoints()
{
  ImGui::Begin("Points");
  ImGui::Text("%d", m_points);
  ImGui::End();
}

void Game::shoot()
{
  auto entity = spawnEntity(m_models[static_cast<size_t>(EntityType::LaserBeam)], m_laserTexture);

  auto &playerPhysics = m_registry.get<Physics>(m_player);

  auto &physics = m_registry.get<Physics>(entity);
  physics.entityType = EntityType::LaserBeam;

  physics.position = playerPhysics.position;
  physics.velocity = glm::vec3(0.0f, 0.0f, m_settings.cannonShootingVelocity);
  physics.rotationAxis= glm::vec3(0.0f, 0.0f, 1.0f);
}

void Game::checkCollision()
{
  auto view = m_registry.view<Physics>();

  m_collided.clear();

  for (int i = 0; i < view.size(); ++i) {
    auto entity1 = view[i];
    auto &physics1 = view.get<Physics>(entity1);
    auto const& type1 = physics1.entityType;

    for (int j = i + 1; j < view.size(); ++j) {
      auto entity2 = view[j];
      auto &physics2 = view.get<Physics>(entity2);
      auto const& type2 = physics2.entityType;

      if (isAsteroid(type1) && isAsteroid(type2))
        continue;

      if (type1 == EntityType::LaserBeam && type2 == EntityType::LaserBeam)
        continue;

      if ((type1 == EntityType::Player && type2 == EntityType::LaserBeam) ||
          (type1 == EntityType::LaserBeam && type2 == EntityType::Player))
        continue;

      bool const collision = hasCollision(physics1, physics2);

      if (!collision)
        continue;

      std::cout << "Collision between " << getEntityTypeName(type1) << " - " << getEntityTypeName(type2) << std::endl;

      if ((type1 == EntityType::LaserBeam && isAsteroid(type2)) ||
          (isAsteroid(type1) && type2 == EntityType::LaserBeam)) {
        m_collided.push_back(std::make_pair(entity1, entity2));
      }
    }
  }

  while (!m_collided.empty()) {
    auto pair = m_collided.back();
    auto entity1 = pair.first;
    auto entity2 = pair.second;

    auto type1 = view.get<Physics>(entity1).entityType;
    auto type2= view.get<Physics>(entity2).entityType;

    auto type = isAsteroid(type1) ? type1 : type2;

    m_registry.destroy(entity1);
    m_registry.destroy(entity2);

    m_collided.pop_back();

    m_points += m_pointsPerAsteroid[static_cast<size_t>(type)];
  }
}

void Game::debugDrawSystem()
{
  ImGuiIO& io = ImGui::GetIO();

  ImGui::Begin("System");
  if (ImGui::Checkbox("Debug boxes", &m_drawDebugBoxes))
  {

  }
  ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);
  ImGui::Text("FPS: %.3f ms", io.Framerate);
  ImGui::End();
}

void Game::debugDrawEntitiesTree()
{
  ImGui::Begin("Entities");

  auto view = m_registry.view<Physics>();

  size_t const count = view.size();

  ImGui::Text("Count: %d", count);

  for (size_t i = 0; i < count; ++i) {
    auto entity = view[i];
    auto &physics = view.get<Physics>(entity);

    if (ImGui::TreeNode((void*)(intptr_t)i, "Entity #%02d (%s)", i, getEntityTypeName(physics.entityType).data()))
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

void Game::debugDrawParams()
{
  ImGui::Begin("Params");
  
  ImGui::PushItemWidth(70.0f);

  ImGui::InputFloat("cannonShootingFrequency", &m_settings.cannonShootingFrequency);
  ImGui::InputFloat("cannonShootingVelocity", &m_settings.cannonShootingVelocity);
  ImGui::InputFloat("spaceshipForwardVelocity", &m_settings.spaceshipForwardVelocity);
  ImGui::InputFloat("asteroidsAngularVelocityRange", &m_settings.asteroidsAngularVelocityRange);
  ImGui::InputFloat("engineThrust", &m_settings.engineThrust);
  ImGui::InputFloat("spaceshipMass", &m_settings.spaceshipMass);
  ImGui::InputFloat("asteroidsAppearanceFrequency", &m_settings.asteroidsAppearanceFrequency);
  ImGui::InputFloat("asteroidsApperanceIncrease", &m_settings.asteroidsApperanceIncrease);

  ImGui::Separator();

  ImGui::InputFloat("scale.AsteroidFragment", &m_scales[static_cast<size_t>(EntityType::AsteroidFragment)]);
  ImGui::InputFloat("scale.AsteroidSmall", &m_scales[static_cast<size_t>(EntityType::AsteroidSmall)]);
  ImGui::InputFloat("scale.AsteroidMedium", &m_scales[static_cast<size_t>(EntityType::AsteroidMedium)]);
  ImGui::InputFloat("scale.AsteroidBig", &m_scales[static_cast<size_t>(EntityType::AsteroidBig)]);
  ImGui::InputFloat("scale.LaserBeam", &m_scales[static_cast<size_t>(EntityType::LaserBeam)]);
  ImGui::InputFloat("scale.Player", &m_scales[static_cast<size_t>(EntityType::Player)]);

  ImGui::Separator();

  ImGui::InputFloat("radius.AsteroidFragment", &m_radiuses[static_cast<size_t>(EntityType::AsteroidFragment)]);
  ImGui::InputFloat("radius.AsteroidSmall", &m_radiuses[static_cast<size_t>(EntityType::AsteroidSmall)]);
  ImGui::InputFloat("radius.AsteroidMedium", &m_radiuses[static_cast<size_t>(EntityType::AsteroidMedium)]);
  ImGui::InputFloat("radius.AsteroidBig", &m_radiuses[static_cast<size_t>(EntityType::AsteroidBig)]);
  ImGui::InputFloat("radius.LaserBeam", &m_radiuses[static_cast<size_t>(EntityType::LaserBeam)]);
  ImGui::InputFloat("radius.Player", &m_radiuses[static_cast<size_t>(EntityType::Player)]);
  
  ImGui::PopItemWidth();

  ImGui::End();
}
