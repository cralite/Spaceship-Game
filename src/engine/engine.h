#ifndef ENGINE_H
#define ENGINE_H

#include "data_types.h"

#include <SDL.h>
#include <glad/glad.h>
#include <glm/matrix.hpp>

#include <map>


class Engine {
 public:
   Engine();
   ~Engine() = default;
   
   void loadTextures();
   void loadModels();
   void prepareScene();
   
   void handleWindowEvent(SDL_Event a_event);
   void handleKeybordEvent();
   
   entt::entity spawnEntity(EngineDataType::EntityType a_entityType);
   void updateCamera(entt::entity& a_player);
   void drawEntities();
   void destroyEntity(entt::entity& a_entity);
   
   void updateAllEntities(entt::entity& a_entity, float a_delta);
   void clearColor(float color[3]);
   void swapWindow();

   //TODO rethink this - maybe separate struct/class for this type of functions?
   void moveEntityLeft(entt::entity& a_entity, float a_delta);
   void moveEntityRight(entt::entity& a_entity, float a_delta);

   EngineDataType::Physics& getPhysics(entt::entity& a_entity);
   bool getKeyStatus(EngineDataType::Key a_key);
   EngineDataType::COLLIDED& getCollided();

 private:
   void setupCamera();
   void updatePlayer(entt::entity& a_entity, float a_delta);
   void updateEntities(float a_delta);

   //TODO the same as below, maybe separate?
   bool hasCollision(EngineDataType::Physics const& entity1, EngineDataType::Physics const& entity2);
   void checkCollision();

 private:
   SDL_Window* m_window{};
   SDL_GLContext m_context{};

   EngineDataType::Shader m_shader{};
   EngineDataType::Camera m_camera{};

   entt::registry m_registry{};
   glm::mat4 m_projectionMatrix{};
   glm::vec3 m_lookDirection{};

   EngineDataType::KEYS m_keys{};
   std::map<EngineDataType::EntityType, EngineDataType::Model> m_models{};
   std::map<EngineDataType::EntityType, EngineDataType::Texture> m_textures{};

   EngineDataType::COLLIDED m_collided{};
};

#endif //ENGINE_H
