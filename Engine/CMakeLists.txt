file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)

message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} "Include/Core/Logger.h" "Source/Core/Logger.cpp" "All.h" "Include/Game/Game.h" "Source/Game/Game.cpp" "Include/Graphics/Window.h" "Source/Graphics/Window.cpp" "Include/Input/InputHandler.h" "Source/Input/InputHandler.cpp" "Include/Input/ActionMap.h"   "Include/Utilities/Scheduler.h" "Include/TickedSystems/TickedSystem.h"  "Include/TickedSystems/GraphicsTicks.h" "Source/TickedSystems/GraphicsTicks.cpp" "Include/Physics/Vector3.h" "Source/Physics/Vector3.cpp")

target_include_directories(Engine
    PUBLIC Include
    PRIVATE Source
)

find_package(glfw3 REQUIRED)
target_link_libraries(Engine PUBLIC
    glfw
)

target_compile_definitions(Engine PUBLIC
    NOMINMAX
)