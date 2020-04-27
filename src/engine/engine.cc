#include "engine.h"
#include "utils.h"

#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <array>

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

void APIENTRY myGlDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

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

Engine::Engine()
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

  glViewport(0, 0, 1280, 720);

  m_shader.program = glCreateProgram();
  utils::load_shader("data/shaders/shader.vert", engineDataType::ShaderType::Vertex, m_shader);
  utils::load_shader("data/shaders/shader.frag", engineDataType::ShaderType::Fragment, m_shader);

  setupCamera();
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(myGlDebugOutput, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}

void Engine::loadTextures()
{
  engineDataType::Texture asteroid = utils::load_texture("data/textures/asteroid.png");
  m_textures[engineDataType::EntityType::AsteroidBig] = asteroid;
  m_textures[engineDataType::EntityType::AsteroidFragment] = asteroid;
  m_textures[engineDataType::EntityType::AsteroidSmall] = asteroid;
  m_textures[engineDataType::EntityType::AsteroidMedium] = asteroid;
  m_textures[engineDataType::EntityType::Player] = utils::load_texture("data/textures/player.png");
  m_textures[engineDataType::EntityType::LaserBeam] = utils::load_texture("data/textures/laser_beam.png");
}

void Engine::loadModels()
{
  m_models[engineDataType::EntityType::AsteroidFragment] = utils::load_model("data/models/asteroid_fragment.obj");
  m_models[engineDataType::EntityType::AsteroidSmall] = utils::load_model("data/models/asteroid_small.obj");
  m_models[engineDataType::EntityType::AsteroidMedium] = utils::load_model("data/models/asteroid_medium.obj");
  m_models[engineDataType::EntityType::AsteroidBig] = utils::load_model("data/models/asteroid_big.obj");
  m_models[engineDataType::EntityType::LaserBeam] = utils::load_model("data/models/laser_beam.obj");
  m_models[engineDataType::EntityType::Player] = utils::load_model("data/models/player.obj");
  m_models[engineDataType::EntityType::Box] = utils::load_model(g_vertices);
}

void Engine::prepareScene()
{
  glDepthMask(true);
  glUseProgram(m_shader.program);

  m_projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
  m_lookDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
}

void Engine::setupCamera()
{
  m_camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
  m_camera.up = glm::vec3(0.0f, 0.0f, 1.0f);
  m_camera.direction = glm::vec3(0.0f, 0.0f, 1.0f);
  m_camera.lookAt = glm::vec3(0.0f, -1.0f, 0.0f);
  m_camera.offset = glm::vec3(0.0f, 50.0f, 18.0f);
}

bool Engine::hasCollision(engineDataType::Physics const& entity1, engineDataType::Physics const& entity2)
{
  auto const type1 = static_cast<size_t>(entity1.entityType);
  auto const type2 = static_cast<size_t>(entity2.entityType);

  auto length = glm::length(entity1.position - entity2.position);
  int l = static_cast<int>(length);
  return length <= (m_radiuses[type1] + m_radiuses[type2]);
}

void Engine::checkCollision()
{
  auto view = m_registry.view<engineDataType::Physics>();

  m_collided.clear();

  for (int i = 0; i < view.size(); ++i) {
    auto entity1 = view[i];
    auto &physics1 = view.get<engineDataType::Physics>(entity1);
    auto const& type1 = physics1.entityType;
  
    for (int j = i + 1; j < view.size(); ++j) {
      auto entity2 = view[j];
      auto &physics2 = view.get<engineDataType::Physics>(entity2);
      auto const& type2 = physics2.entityType;
  
      if (type1 == type2)
        continue;
  
      bool const collision = hasCollision(physics1, physics2);
  
      if (!collision)
        continue;
  
      m_collided.push_back(std::make_pair(entity1, entity2));
    }
  }
}

void Engine::handleWindowEvent(SDL_Event a_event)
{
  //switch (a_event) {

  //};
}

void Engine::handleKeybordEvent()
{
  auto getKeys = [this](SDL_KeyboardEvent a_key, bool a_pressed) {
    if (a_key.keysym.sym == SDLK_SPACE)
      m_keys[static_cast<size_t>(engineDataType::Key::Space)] = a_pressed;
    else if (a_key.keysym.sym == SDLK_LEFT)
      m_keys[static_cast<size_t>(engineDataType::Key::Left)] = a_pressed;
    else if (a_key.keysym.sym == SDLK_RIGHT)
      m_keys[static_cast<size_t>(engineDataType::Key::Right)] = a_pressed;
  };

  SDL_Event event{};
  if (SDL_PollEvent(&event)) {

    switch (event.type) {
    case SDL_QUIT:
      m_keys[static_cast<size_t>(engineDataType::Key::Quit)] = true;
      break;
      //case SDL_WINDOWEVENT:
        //handleWindowEvent()
    case SDL_KEYUP:
      getKeys(event.key, false);
      break;
    case SDL_KEYDOWN:
      getKeys(event.key, true);
      break;
    };
  }
}

entt::entity Engine::spawnEntity(engineDataType::EntityType a_entityType)
{
  auto entity = m_registry.create();
  m_registry.assign<engineDataType::Model>(entity, m_models[a_entityType]);
  m_registry.assign<engineDataType::Texture>(entity, m_textures[a_entityType]);
  engineDataType::Physics physics{};
  physics.entityType = a_entityType;
  m_registry.assign<engineDataType::Physics>(entity, physics);

  return entity;
}

void Engine::updateCamera(entt::entity& a_player)
{
  auto& physics = m_registry.get<engineDataType::Physics>(a_player);
  m_camera.pos = physics.position + m_camera.offset;
  m_camera.pos.x = 0.0f;

  glm::mat4 view{ glm::lookAt(m_camera.pos, m_camera.pos + m_camera.lookAt, m_camera.up) };

  uint32_t const modelLocation = glGetUniformLocation(m_shader.program, "view");
  uint32_t const projectionLocation = glGetUniformLocation(m_shader.program, "projection");

  glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
}

void Engine::drawEntities()
{
  auto view = m_registry.view<engineDataType::Texture, engineDataType::Model, engineDataType::Physics>();

  int modelLoc{ glGetUniformLocation(m_shader.program, "model") };

  for (auto entity : view) {
    auto& model = view.get<engineDataType::Model>(entity);
    auto& texture = view.get<engineDataType::Texture>(entity);
    auto& physics = view.get<engineDataType::Physics>(entity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.texture);
    glBindVertexArray(model.vao);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(physics.modelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, model.vertices);

    //TODO drawdebugboxes
    //if (m_drawDebugBoxes) {
    if (false) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      auto& boxModel = m_models[engineDataType::EntityType::Box];

      glBindVertexArray(boxModel.vao);

      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(physics.debugBoxMatrix));

      glDrawArrays(GL_TRIANGLES, 0, model.vertices);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }
}

void Engine::destroyEntity(entt::entity& a_entity)
{
  m_registry.destroy(a_entity);
}

void Engine::updateAllEntities(entt::entity& a_entity, float a_delta)
{
  updatePlayer(a_entity, a_delta);
  updateEntities(a_delta);
}

void Engine::clearColor(float color[3])
{
  glClearColor(color[0], color[1], color[2], 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::swapWindow()
{
  SDL_GL_SwapWindow(m_window);
}

void Engine::moveEntityLeft(entt::entity& a_entity, float a_delta)
{
  auto& physics = m_registry.get<engineDataType::Physics>(a_entity);
  physics.position -= m_lookDirection * m_camera.speed * a_delta;
}

void Engine::moveEntityRight(entt::entity& a_entity, float a_delta)
{
  auto& physics = m_registry.get<engineDataType::Physics>(a_entity);
  physics.position += m_lookDirection * m_camera.speed * a_delta;
}

engineDataType::Physics& Engine::getPhysics(entt::entity& a_entity)
{
  return m_registry.get<engineDataType::Physics>(a_entity);
}

bool Engine::getKeyStatus(engineDataType::Key a_key)
{
  return m_keys[static_cast<size_t>(a_key)];
}

engineDataType::Collided& Engine::getCollided()
{
  checkCollision();
  return m_collided;
}

void Engine::setScales(const SettingsArray& a_array)
{
  m_scales = a_array;
}

void Engine::setRadiuses(const SettingsArray& a_array)
{
  m_radiuses = a_array;
}

void Engine::updatePlayer(entt::entity& a_entity, float a_delta)
{
  auto& model = m_registry.get<engineDataType::Model>(a_entity);
  auto& texture = m_registry.get<engineDataType::Texture>(a_entity);
  auto& physics = m_registry.get<engineDataType::Physics>(a_entity);

  //TODO deliver settings to engine
  physics.position += 1.1f * m_camera.direction * a_delta;

  physics.modelMatrix = glm::mat4(1.0f);
  physics.modelMatrix = glm::translate(physics.modelMatrix, physics.position);

  physics.debugBoxMatrix = glm::mat4(1.0f);
  physics.debugBoxMatrix = glm::translate(physics.debugBoxMatrix, physics.position);
  physics.debugBoxMatrix = glm::scale(physics.debugBoxMatrix, glm::vec3(m_radiuses[static_cast<size_t>(physics.entityType)]));
}

void Engine::updateEntities(float a_delta)
{
  auto view = m_registry.view<engineDataType::Physics>();

  for (auto entity : view) {
    auto& physics = view.get<engineDataType::Physics>(entity);

    if (physics.entityType == engineDataType::EntityType::Player)
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