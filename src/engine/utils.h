#ifndef ULILS_H
#define ULILS_H

#include "data_types.h"

#include <optional>
#include <string>
#include <vector>

namespace Utils {

  std::optional<std::string> open_file(std::string_view a_path);

  EngineDataType::Texture load_texture(std::string_view a_path);
  void load_shader(std::string_view a_path, EngineDataType::ShaderType a_type, EngineDataType::Shader& a_shader);
  EngineDataType::Model load_model(const std::vector<float>& a_data);
  EngineDataType::Model load_model(std::string_view a_path);

}; // namespace utils

#endif //ULILS_H
