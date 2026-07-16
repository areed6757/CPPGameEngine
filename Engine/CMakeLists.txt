file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)

message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} )

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

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:/Od /Zi>
    $<$<CONFIG:Release>:/O2 /DNDEBUG>
)
target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/default.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/default.frag
        $<TARGET_FILE_DIR:Game>
)