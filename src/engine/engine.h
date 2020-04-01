#ifndef ENGINE_H
#define ENGINE_H

#include "data_types.h"

#include <SDL.h>
#include <glad/glad.h>
#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include <map>

class Engine {
 public:
   Engine();
   ~Engine() = default;
   
   void loadTextures();
   void loadModels();
   void prepareScene();

   bool hasCollision(EngineDataType::Physics const& entity1, EngineDataType::Physics const& entity2);
   std::vector<std::pair<entt::entity, entt::entity>> checkCollision();
   
   void handleWindowEvent(SDL_Event a_event);
   void handleKeybordEvent(SDL_KeyboardEvent a_key, bool a_pressed);
   
   entt::entity spawnEntity(EngineDataType::EntityType a_entityType);
   void updateCamera(entt::entity& a_player);
   void drawEntities();
   
   void updateAllEntities(entt::entity& a_entity, float a_delta);
   void clearColor(float color[3]);
   void swapWindow();

   EngineDataType::Physics& getPhysics(entt::entity& a_entity);

 private:
   void setupCamera();
   void updatePlayer(entt::entity& a_entity, float a_delta);
   void updateEntities(float a_delta);

 private:
   SDL_Window* m_window{};
   SDL_GLContext m_context{};

   EngineDataType::Shader m_shader{};
   EngineDataType::Camera m_camera{};

   entt::registry m_registry{};
   glm::mat4 m_projectionMatrix{};

   std::map<EngineDataType::EntityType, EngineDataType::Model> m_models{};
   std::map<EngineDataType::EntityType, EngineDataType::Texture> m_textures{};

   std::vector<std::pair<entt::entity, entt::entity>> m_collided{};
};

#endif //ENGINE_H
