#include "utils.h"
#include <fstream>
#include <iostream>

std::optional<std::string> GameUtils::open_file(std::string_view a_path)
{
  std::fstream fs{};
  std::string output{};
  fs.open(a_path.data(), std::ios::in);
  if (fs.is_open()) {
    while (fs.good()) {
      std::string str{};
      std::getline(fs, str);
      output += str + "\n";
    }
    fs.close();
  }
  else {
    std::cerr << "cannot open file" << std::endl;
    return {};
  }
  return output;
}