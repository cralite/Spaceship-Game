#ifndef GAMEULILS_H
#define GAMEULILS_H

#include "data_types.h"

#include <optional>
#include <string>
#include <vector>

namespace GameUtils {
  //TODO temporary, to remove
  std::optional<std::string> open_file(std::string_view a_path);

}; // namespace GameUtils

#endif //GAMEULILS_H
