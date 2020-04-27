set(IMGUI_FILES
	imgui/imgui.cpp
	imgui/imgui.h
	imgui/imgui_draw.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_impl_opengl3.h
	imgui/imgui_impl_sdl.cpp
	imgui/imgui_impl_sdl.h
	imgui/imgui_internal.h
	imgui/imgui_widgets.cpp
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
)

set(ENGINE_SRC
	engine/data_types.h
	engine/engine.h
	engine/engine.cc
	engine/utils.h
	engine/utils.cc
)

set(OBJ_LOADER
	tinyobjloader/tiny_obj_loader.cc
	tinyobjloader/tiny_obj_loader.h
)

set(ENGINE
	${IMGUI_FILES}
	${ENGINE_SRC}
	${OBJ_LOADER}
)

set(GAME_SRC
	main.cc
	game.h
	game.cc
	data_types.h
)

add_executable(SpaceshipGame
	${GAME_SRC}
	${ENGINE}
)

target_include_directories(SpaceshipGame
	PRIVATE ${Stb_INCLUDE_DIR} imgui tinyobjloader
)

target_compile_definitions(SpaceshipGame
	PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD IMGUI_DISABLE_INCLUDE_IMCONFIG_H
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