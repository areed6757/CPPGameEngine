file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)

add_executable(Game main.cpp ${GAME_SOURCES} ${GAME_HEADERS} "Include/Components/Renderable.h" "Include/Graphics/MeshRegistery.h" "Include/TextureRegistery.h" "Include/Graphics/TextureID.h")

target_include_directories(Game
    PUBLIC Include
    PRIVATE Source
)

target_link_libraries(Game PRIVATE Engine)