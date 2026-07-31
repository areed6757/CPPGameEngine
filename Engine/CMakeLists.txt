file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)

message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS}  "Include/Core/Application.h" "Source/Core/Application.cpp" "Include/Engine.h" "Include/Events/Event.h" "Include/Events/KeyEvent.h" "Include/Events/MouseEvent.h" "Include/Physics/AABBTree.h" "Source/Physics/AABBTree.cpp")

target_include_directories(Engine
    PUBLIC Include
    PRIVATE Source
)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_link_libraries(Engine PUBLIC
    glfw
    glm::glm
    spdlog::spdlog
)

target_link_libraries(Engine PRIVATE imgui::imgui)

target_compile_definitions(Engine PUBLIC
    NOMINMAX
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:/Od /Zi>
    $<$<CONFIG:Release>:/O2 /DNDEBUG>
)
target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_definitions(Engine PUBLIC
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

target_precompile_headers(Engine PRIVATE
    Include/Engine.h
)

set_source_files_properties(
    Source/ThirdParty/glad/glad.c
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)