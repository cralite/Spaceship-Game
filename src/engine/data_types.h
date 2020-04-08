#ifndef E_DATA_TYPE_H
#define E_DATA_TYPE_H

#include <cstdint>
#include <array>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <entt/entt.hpp>

namespace EngineDataType {

  enum class EntityType {
    AsteroidFragment,
    AsteroidSmall,
    AsteroidMedium,
    AsteroidBig,
    LaserBeam,
    Player,
    Box,
    Count
  };

  enum class ShaderType {
    Vertex,
    Fragment
  };

  struct Model {
    uint32_t vao{};
    uint32_t vertices{};
  };

  struct Shader {
    uint32_t program{};
  };

  struct Texture {
    uint32_t texture{};
  };

  struct Camera {
    glm::vec3 pos{};
    glm::vec3 up{};
    glm::vec3 view{};
    glm::vec3 offset{};
    glm::vec3 lookAt{};
    glm::vec3 direction{};
    float speed{ 15.00f };
  };

  struct Physics {
    glm::mat4 modelMatrix{};
    glm::mat4 debugBoxMatrix{};
    glm::vec3 position{};
    glm::vec3 velocity{};
    glm::vec3 acceleration{};
    glm::vec3 rotationAxis{};
    float rotationAngle{};
    float rotationVelocity{};
    EntityType entityType{};
  };


  enum class Key {
    Left,
    Right,
    Space,
    Quit,
    Count,
  };

  using KEYS = std::array<bool, static_cast<size_t>(Key::Count)>;
  using COLLIDED = std::vector<std::pair<entt::entity, entt::entity>>;
}

#endif // E_DATA_TYPE_H