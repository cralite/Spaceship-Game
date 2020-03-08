#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>

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

#endif //DATA_TYPES_H