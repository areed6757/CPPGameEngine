file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)

message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS}  "Include/Graphics/DebugLineRenderer.h" "Source/Graphics/DebugLineRenderer.cpp")

target_include_directories(Engine
    PUBLIC Include
    PRIVATE Source
)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC
    glfw
    glm::glm
    spdlog::spdlog
)

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