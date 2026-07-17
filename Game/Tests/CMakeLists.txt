find_package(Catch2 3 REQUIRED)
file(GLOB_RECURSE GAME_LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Game/Source/*.cpp)
list(FILTER GAME_LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
add_executable(GameTests
    test_ecswrapper.cpp
    test_movementticks.cpp
    test_performance.cpp
    ${GAME_LIB_SOURCES}
)
target_include_directories(GameTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Game/Include
    ${CMAKE_SOURCE_DIR}/Game/Source
)
target_link_libraries(GameTests PRIVATE Engine Catch2::Catch2WithMain)

enable_testing()
add_test(NAME GameTests COMMAND GameTests)