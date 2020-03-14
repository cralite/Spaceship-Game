#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

enum class ShaderType {
  eVertex,
  eFragment
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
  float speed{ 1.00f };
};

struct Physics {
  glm::mat4 modelMatrix{};
  glm::vec3 position{};
  glm::vec3 velocity{};
  glm::vec3 acceleration{};
  glm::vec3 rotationAxis{};
  float rotationAngle{};
  float rotationVelocity{};
  bool player{};
};

enum class Key {
  eLeft,
  eRight,
  eUp,
  eDown,
  eSpace,
  eCount,
};



#endif //DATA_TYPES_H