#ifndef ULILS_H
#define ULILS_H

#include "data_types.h"

#include <optional>
#include <string>
#include <vector>

namespace Utils {

  std::optional<std::string> open_file(std::string_view a_path);

  Texture load_texture(std::string_view a_path);
  void load_shader(std::string_view a_path, ShaderType a_type, Shader& a_shader);
  Model load_model(const std::vector<float>& a_data);

}; // namespace utils

#endif //ULILS_H
