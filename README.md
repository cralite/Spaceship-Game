# Spaceship-Game

To build this game you have to set up `vcpkg`:
https://github.com/microsoft/vcpkg

- Set `VCPKG_ROOT` environment variable pointed to directory containing vcpkg
- Set `VCPKG_DEFAULT_TRIPLET` to `x64-windows`
- Build `vcpkg`
- Install these packages:
  ```
  vcpkg install sdl2
  vcpkg install glm
  vcpkg install nlohmann-json
  vcpkg install glad
  vcpkg install tinyobjloader
  vcpkg install entt
  ```
- Run `cmake-gui` and create visual studio solution
- Set working directory to solution root directory

3D models was bought from:
https://sketchfab.com/3d-models/space-elements-463f76fc7ae04ff0a7c1ba7cd19225ec
