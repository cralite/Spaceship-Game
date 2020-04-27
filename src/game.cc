#include "game.h"

#include <nlohmann/json.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <iostream>
#include <math.h>
#include <chrono>
#include <cassert>
#include <random>

#include "engine/utils.h"

std::random_device g_rd;
std::mt19937 g_gen{ g_rd() };
std::uniform_real_distribution<float> g_asteroidAngleVelocity(10.05f, 30.5f);

Game::Game()
{

  //IMGUI_CHECKVERSION();
  //ImGui::CreateContext();
  //ImGui::StyleColorsDark();

  //ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
  //ImGui_ImplOpenGL3_Init("#version 150");

  m_engine.loadTextures();
  m_engine.loadModels();

  loadSettings();
}



void Game::setupPlayer()
{
  m_player = m_engine.spawnEntity(engineDataType::EntityType::Player);
}

void Game::spawnAsteroids()
{
  //for (size_t i = 0; i < m_asteroidsAppearanceFrequency; ++i)
    spawnAsteroid();
}

void Game::spawnAsteroid()
{
  std::uniform_int_distribution<size_t> randomAsteroidType(static_cast<size_t>(engineDataType::EntityType::AsteroidFragment), 
    static_cast<size_t>(engineDataType::EntityType::AsteroidBig));

  auto const modelIndex = randomAsteroidType(g_gen);
  auto const type = static_cast<engineDataType::EntityType>(modelIndex);

  auto asteroid = m_engine.spawnEntity(engineDataType::EntityType::AsteroidBig);

  auto& playerPhysics = m_engine.getPhysics(m_player);
  auto &playerPos = playerPhysics.position;
  auto &physics = m_engine.getPhysics(asteroid);

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

  if (auto configData = utils::open_file("data/configs/config.json")) {
    json config{ json::parse((*configData).data()) };
    m_settings.cannonShootingFrequency = config["cannonShootingFrequency"].get<float>();
    m_settings.cannonShootingVelocity = config["cannonShootingVelocity"].get<float>();
    m_settings.spaceshipForwardVelocity = config["spaceshipForwardVelocity"].get<float>();
    m_settings.engineThrust = config["engineThrust"].get<float>();
    m_settings.spaceshipMass = config["spaceshipMass"].get<float>();
    m_settings.asteroidsAppearanceFrequency = config["asteroidsAppearanceFrequency"].get<float>();
    m_settings.asteroidsApperanceIncrease = config["asteroidsApperanceIncrease"].get<float>();

    auto scales = config["scales"];
    m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidFragment)] = scales["AsteroidFragment"].get<float>();
    m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidSmall)] = scales["AsteroidSmall"].get<float>();
    m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidMedium)] = scales["AsteroidMedium"].get<float>();
    m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidBig)] = scales["AsteroidBig"].get<float>();
    m_scales[static_cast<size_t>(engineDataType::EntityType::LaserBeam)] = scales["LaserBeam"].get<float>();
    m_scales[static_cast<size_t>(engineDataType::EntityType::Player)] = scales["Player"].get<float>();

    auto radius = config["radius"];
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidFragment)] = radius["AsteroidFragment"].get<float>();
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidMedium)] = radius["AsteroidMedium"].get<float>();
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidSmall)] = radius["AsteroidSmall"].get<float>();
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidBig)] = radius["AsteroidBig"].get<float>();
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::LaserBeam)] = radius["LaserBeam"].get<float>();
    m_radiuses[static_cast<size_t>(engineDataType::EntityType::Player)] = scales["Player"].get<float>();


    auto points = config["points"];
    m_pointsPerAsteroid[static_cast<size_t>(engineDataType::EntityType::AsteroidFragment)] = points["AsteroidFragment"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(engineDataType::EntityType::AsteroidMedium)] = points["AsteroidMedium"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(engineDataType::EntityType::AsteroidSmall)] = points["AsteroidSmall"].get<int32_t>();
    m_pointsPerAsteroid[static_cast<size_t>(engineDataType::EntityType::AsteroidBig)] = points["AsteroidBig"].get<int32_t>();

    m_engine.setScales(m_scales);
    m_engine.setRadiuses(m_radiuses);
  } else {
      std::cerr << "cant load config";
  }
}

void Game::saveSettings()
{

}

bool Game::isAsteroid(engineDataType::EntityType a_type)
{
  return a_type == engineDataType::EntityType::AsteroidBig || a_type == engineDataType::EntityType::AsteroidFragment ||
    a_type == engineDataType::EntityType::AsteroidMedium || a_type == engineDataType::EntityType::AsteroidSmall;
}



void Game::gameLoop()
{
  glEnable(GL_DEPTH_TEST);


  using clock_t = std::chrono::high_resolution_clock;
  auto start = clock_t::now();
  using duration = std::chrono::duration<double, std::milli>;
  double asteroidSpawnTime{};
  double lasersSpawnTime{};

  double laserTimeDiff = 1.0 / m_settings.cannonShootingFrequency;

  bool quit{};
  m_engine.prepareScene();
  float clearColor[3]{ 0.2f, 0.3f, 0.3f };

  reset();

  while (!quit) {
    m_engine.handleKeybordEvent();

    //TODO Fix imgui
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplSDL2_NewFrame(m_window);
    //ImGui::NewFrame();
    m_engine.clearColor(clearColor);

    const auto now = clock_t::now();
    const duration deltaDuration = now - start;
    start = now;
    double delta{ deltaDuration.count() / 1000.0 };

    if (m_gameState == GameState::Playing) {
      asteroidSpawnTime += delta;

      if (asteroidSpawnTime >= 1.0f) {
        spawnAsteroids();
        asteroidSpawnTime = 0.0f;
      }

      updateInput(delta);
      quit = m_engine.getKeyStatus(engineDataType::Key::Quit);

      if (m_shoot) {
        if (lasersSpawnTime >= laserTimeDiff)
          lasersSpawnTime = 0.0f;

        if (lasersSpawnTime == 0.0)
          shoot();

        lasersSpawnTime += delta;
      }
      else
        lasersSpawnTime = 0.0;

      m_engine.updateAllEntities(m_player, delta);
      checkCollision();
    } else {
      drawEndGame();
    }

    m_engine.updateCamera(m_player);

    m_engine.drawEntities();

    //TODO imgui
    //drawPoints();

    //debugDrawSystem();
    //debugDrawEntitiesTree();
    //debugDrawParams();

    //ImGui::Render();

    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    m_engine.swapWindow();
  }

  saveSettings();
}

std::string_view Game::getengineDataType(engineDataType::EntityType a_type)
{
  switch (a_type)
  {
    case engineDataType::EntityType::AsteroidFragment: return "AsteroidFragment";
    case engineDataType::EntityType::AsteroidSmall: return "AsteroidSmall";
    case engineDataType::EntityType::AsteroidMedium: return "AsteroidMedium";
    case engineDataType::EntityType::AsteroidBig: return "AsteroidBig";
    case engineDataType::EntityType::Player: return "Player";
    case engineDataType::EntityType::LaserBeam: return "LaserBeam";
  }

  return "<unknown>";
}

void Game::updateInput(float a_delta)
{ 
  if (m_engine.getKeyStatus(engineDataType::Key::Left))
    m_engine.moveEntityLeft(m_player, a_delta);
  
  if (m_engine.getKeyStatus(engineDataType::Key::Right))
    m_engine.moveEntityRight(m_player, a_delta);
  
  m_shoot = m_engine.getKeyStatus(engineDataType::Key::Space);
}



void Game::drawPoints()
{
  ImGui::Begin("Points");
  ImGui::Text("%d", m_points);
  ImGui::End();
}

void Game::drawEndGame()
{
  ImGui::Begin("##EndGame");
  ImGui::Text("You lost!");
  if (ImGui::Button("Restart"))
    reset();
  ImGui::End();
}

void Game::shoot()
{
  auto entity = m_engine.spawnEntity(engineDataType::EntityType::LaserBeam);

  auto &playerPhysics = m_engine.getPhysics(m_player);

  auto &physics = m_engine.getPhysics(entity);

  physics.position = playerPhysics.position;
  physics.velocity = glm::vec3(0.0f, 0.0f, m_settings.cannonShootingVelocity);
  physics.rotationAxis= glm::vec3(0.0f, 0.0f, 1.0f);
}

void Game::checkCollision()
{
  auto& collided = m_engine.getCollided();

  for (auto it : collided) {
    auto entity1 = it.first;
    auto type1 = m_engine.getPhysics(entity1).entityType;

    auto entity2 = it.second;
    auto type2 = m_engine.getPhysics(entity2).entityType;

    if ((type1 == engineDataType::EntityType::Player && type2 == engineDataType::EntityType::LaserBeam) ||
      (type1 == engineDataType::EntityType::LaserBeam && type2 == engineDataType::EntityType::Player))  
      continue;

    if ((isAsteroid(type1) && type2 == engineDataType::EntityType::LaserBeam) ||
      (type1 == engineDataType::EntityType::LaserBeam && isAsteroid(type2))) {
      auto asteroid = isAsteroid(type1) ? type1 : type2;
      m_engine.destroyEntity(entity1);
      m_engine.destroyEntity(entity2);
      m_points += m_pointsPerAsteroid[static_cast<size_t>(asteroid)];
    }
  }
}

void Game::reset()
{
  m_gameState = GameState::Playing;
  m_asteroidsAppearanceFrequency = m_settings.asteroidsAppearanceFrequency;
  m_points = 0;

  m_engine.clearRegistry();

  setupPlayer();
  spawnAsteroids();
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
  //TODO imgui
  //ImGui::Begin("Entities");
  //
  //auto view = m_registry.view<Physics>();
  //
  //size_t const count = view.size();
  //
  //ImGui::Text("Count: %d", count);
  //
  //for (size_t i = 0; i < count; ++i) {
  //  auto entity = view[i];
  //  auto &physics = view.get<Physics>(entity);
  //
  //  if (ImGui::TreeNode((void*)(intptr_t)i, "Entity #%02d (%s)", i, getengineDataType::EntityTypeName(physics.engineDataType::EntityType).data()))
  //  {
  //    ImGui::InputFloat3("position", glm::value_ptr(physics.position));
  //    ImGui::InputFloat3("velocity", glm::value_ptr(physics.velocity));
  //    ImGui::InputFloat3("rotationAxis", glm::value_ptr(physics.rotationAxis));
  //    ImGui::InputFloat("rotationAngle", &physics.rotationAngle);
  //    ImGui::InputFloat("rotationVelocity", &physics.rotationVelocity);
  //    ImGui::TreePop();
  //  }
  //
  //}
  //
  //ImGui::End();
  //
  //ImGui::Begin("Camera");
  //ImGui::InputFloat3("up", glm::value_ptr(m_camera.up));
  //ImGui::InputFloat3("position", glm::value_ptr(m_camera.pos));
  //ImGui::InputFloat3("offset", glm::value_ptr(m_camera.offset));
  //ImGui::InputFloat3("lookAt", glm::value_ptr(m_camera.lookAt));
  //ImGui::End();
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

  ImGui::InputFloat("scale.AsteroidFragment", &m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidFragment)]);
  ImGui::InputFloat("scale.AsteroidSmall", &m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidSmall)]);
  ImGui::InputFloat("scale.AsteroidMedium", &m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidMedium)]);
  ImGui::InputFloat("scale.AsteroidBig", &m_scales[static_cast<size_t>(engineDataType::EntityType::AsteroidBig)]);
  ImGui::InputFloat("scale.LaserBeam", &m_scales[static_cast<size_t>(engineDataType::EntityType::LaserBeam)]);
  ImGui::InputFloat("scale.Player", &m_scales[static_cast<size_t>(engineDataType::EntityType::Player)]);

  ImGui::Separator();

  ImGui::InputFloat("radius.AsteroidFragment", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidFragment)]);
  ImGui::InputFloat("radius.AsteroidSmall", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidSmall)]);
  ImGui::InputFloat("radius.AsteroidMedium", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidMedium)]);
  ImGui::InputFloat("radius.AsteroidBig", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::AsteroidBig)]);
  ImGui::InputFloat("radius.LaserBeam", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::LaserBeam)]);
  ImGui::InputFloat("radius.Player", &m_radiuses[static_cast<size_t>(engineDataType::EntityType::Player)]);
  
  ImGui::PopItemWidth();

  ImGui::End();
}
