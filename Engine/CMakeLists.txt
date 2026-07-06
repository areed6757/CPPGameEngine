file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h)

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} "Include/Core/Logger.h" "Source/Core/Logger.cpp" "All.h" "Include/Game/Game.h" "Source/Game/Game.cpp")

target_include_directories(Engine PUBLIC Include)

target_compile_definitions(Engine PUBLIC
    NOMINMAX
)