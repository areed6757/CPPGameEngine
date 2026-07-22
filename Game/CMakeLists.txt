file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.c)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS Include/*.h Source/*.h)
file(GLOB SHADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.frag)
file(GLOB IMAGE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Images/*.png)

add_executable(Game main.cpp ${GAME_SOURCES} ${GAME_HEADERS}  "Include/Test/ThrusterTest.h" "Source/Test/ThrusterTest.cpp")

target_include_directories(Game
    PUBLIC Include
    PRIVATE Source
)

target_link_libraries(Game PRIVATE Engine)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Game>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILES} $<TARGET_FILE_DIR:Game>/Shaders
)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Game>/Images
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IMAGE_FILES} $<TARGET_FILE_DIR:Game>/Images
)