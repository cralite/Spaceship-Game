#include "utils.h"

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int VERTEX_LOCATION = 0;
const int TEXTURE_LOCATION = 1;
const int COLOR_LOCATION = 2;


std::optional<std::string> Utils::open_file(std::string_view a_path)
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
  } else {
    std::cerr << "cannot open file" << std::endl;
    return {};
  }
  return output;
}

Texture Utils::load_texture(std::string_view a_path)
{
  stbi_set_flip_vertically_on_load(true);

  int width{}, height{}, nrChannels{};
  unsigned char* data = stbi_load(a_path.data(), &width, &height, &nrChannels, 0);
  if (!data) {
    std::cerr << "cannot load texture" << std::endl;
    return {};
  }

  Texture texture{};
  glGenTextures(1, &texture.texture);
  glBindTexture(GL_TEXTURE_2D, texture.texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const auto format = nrChannels == 4 ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  return texture;
}

void Utils::load_shader(std::string_view a_path, ShaderType a_type, Shader& a_shader)
{
  uint32_t newShader{};
  switch (a_type) {
    case ShaderType::eVertex:
      newShader = glCreateShader(GL_VERTEX_SHADER);
      break;
    case ShaderType::eFragment:
      newShader = glCreateShader(GL_FRAGMENT_SHADER);
      break;
  }

  if (auto shaderSource = open_file(a_path)) {
    int status{};
    char log[512]{};

    const char* const shaderData = (*shaderSource).data();
    glShaderSource(newShader, 1, &shaderData, nullptr);
    glCompileShader(newShader);

    glGetShaderiv(newShader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(newShader, 512, nullptr, log);

    if (!status) {
      std::cerr << "Error compiling shader " << a_path << ":\n";
      std::cerr << log << std::endl;
    }

    assert(status);

    glAttachShader(a_shader.program, newShader);
  }

  glLinkProgram(a_shader.program);
  glDeleteShader(newShader);
}

Model Utils::load_model(const std::vector<float>& a_data)
{
  uint32_t vbo{};

  Model model{};
  glGenVertexArrays(1, &model.vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(model.vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * a_data.size(), a_data.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VERTEX_LOCATION);

  glVertexAttribPointer(TEXTURE_LOCATION, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(12));
  glEnableVertexAttribArray(TEXTURE_LOCATION);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  model.vertices = a_data.size() / 5;

  return model;
}

Model Utils::load_model(std::string_view a_path)
{
  tinyobj::attrib_t attribs{};
  std::vector<tinyobj::shape_t> shapes{};

  std::string warnings{};
  std::string errors{};

  bool const loaded = tinyobj::LoadObj(&attribs, &shapes, nullptr, &warnings, &errors, a_path.data());

  if (!errors.empty())
    std::cout << "Errors: " << errors << std::endl;

  if (!warnings.empty())
    std::cout << "Warning: " << warnings << std::endl;

  if (!loaded)
    return {};

  std::vector<float> modelVertices{};

  size_t shapesCount = shapes.size();

  // loop over shapes
  for (size_t s = 0; s < shapesCount; ++s) {
    size_t indexOffset{};

    auto const& shape = shapes[s];
    auto const& mesh = shape.mesh;

    size_t const facesSize = mesh.num_face_vertices.size();

    // loop over faces
    for (size_t f = 0; f < facesSize; ++f) {
      int const verticesSizePerFace = mesh.num_face_vertices[f];

      // loop over vertices in the face
      for (size_t vertex = 0; vertex < verticesSizePerFace; ++vertex) {
        auto& indices = mesh.indices;
        auto& vertices = attribs.vertices;
        auto& texCoords = attribs.texcoords;

        tinyobj::index_t const index = indices[indexOffset + vertex];

        modelVertices.push_back(vertices[3 * index.vertex_index + 0]);
        modelVertices.push_back(vertices[3 * index.vertex_index + 1]);
        modelVertices.push_back(vertices[3 * index.vertex_index + 2]);

        modelVertices.push_back(texCoords[2 * index.texcoord_index + 0]);
        modelVertices.push_back(texCoords[2 * index.texcoord_index + 1]);
      }

      indexOffset += verticesSizePerFace;
    }
  }

  return load_model(modelVertices);
}
