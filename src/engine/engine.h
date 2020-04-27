#ifndef ENGINE_H
#define ENGINE_H

#include "data_types.h"

#include <SDL.h>
#include <glad/glad.h>
#include <glm/matrix.hpp>

#include <map>

using SettingsArray = std::array<float, static_cast<size_t>(engineDataType::EntityType::Count)>;

class Engine {
 public:
   Engine();
   ~Engine() = default;
   
   void loadTextures();
   void loadModels();
   void prepareScene();
   
   void handleWindowEvent(SDL_Event a_event);
   void handleKeybordEvent();
   
   entt::entity spawnEntity(engineDataType::EntityType a_entityType);
   void updateCamera(entt::entity& a_player);
   void drawEntities();
   void destroyEntity(entt::entity& a_entity);
   
   void updateAllEntities(entt::entity& a_entity, float a_delta);
   void clearColor(float color[3]);
   void swapWindow();

   //TODO rethink this - maybe separate struct/class for this type of functions?
   void moveEntityLeft(entt::entity& a_entity, float a_delta);
   void moveEntityRight(entt::entity& a_entity, float a_delta);

   engineDataType::Physics& getPhysics(entt::entity& a_entity);
   bool getKeyStatus(engineDataType::Key a_key);
   engineDataType::Collided& getCollided();

   void setScales(const SettingsArray& a_array);
   void setRadiuses(const SettingsArray& a_array);


 private:
   void setupCamera();
   void updatePlayer(entt::entity& a_entity, float a_delta);
   void updateEntities(float a_delta);

   //TODO the same as below, maybe separate?
   bool hasCollision(engineDataType::Physics const& entity1, engineDataType::Physics const& entity2);
   void checkCollision();

 private:
   SDL_Window* m_window{};
   SDL_GLContext m_context{};


   engineDataType::Shader m_shader{};
   engineDataType::Camera m_camera{};

   entt::registry m_registry{};
   glm::mat4 m_projectionMatrix{};
   glm::vec3 m_lookDirection{};

   engineDataType::Keys m_keys{};
   std::map<engineDataType::EntityType, engineDataType::Model> m_models{};
   std::map<engineDataType::EntityType, engineDataType::Texture> m_textures{};
   SettingsArray m_scales{};
   SettingsArray m_radiuses{};

   engineDataType::Collided m_collided{};
};

#endif //ENGINE_H
