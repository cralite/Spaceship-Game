add_executable(SpaceshipGame
	main.cc
	game.h
	game.cc
)

target_include_directories(SpaceshipGame
	PRIVATE ${Stb_INCLUDE_DIR}
)

target_link_libraries(SpaceshipGame
	PRIVATE
	OpenGL::GL
	SDL2::SDL2 SDL2::SDL2main
	glm
	spdlog::spdlog spdlog::spdlog_header_only
	assimp::assimp
	nlohmann_json::nlohmann_json
	glad::glad
)