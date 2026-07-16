file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)
file(GLOB SHADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.frag)
file(GLOB IMAGE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Images/*.png)

message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS}  "Source/ThirdParty/stb/stb.cpp" "Include/Graphics/Texture.h" "Source/Graphics/Texture.cpp")

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
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Game>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILES} $<TARGET_FILE_DIR:Game>/Shaders
)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Game>/Images
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IMAGE_FILES} $<TARGET_FILE_DIR:Game>/Images
)