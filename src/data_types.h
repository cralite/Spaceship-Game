#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>
#include <glm/vec3.hpp>

enum class ShaderType {
  eVertex,
  eFragment
};

struct Model {
  uint32_t m_vao{};
};

struct Shader {
  uint32_t m_program{};
};

struct Texture {
  uint32_t m_texture{};
};

struct Camera {
  glm::vec3 m_pos{};
  glm::vec3 m_up{};
  glm::vec3 m_direction{};
  glm::vec3 m_right{};
  glm::vec3 m_view{};
  float m_speed{ 0.05f };
};

#endif //DATA_TYPES_H