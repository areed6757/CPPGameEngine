find_package(Catch2 3 REQUIRED)

add_executable(EngineTests
    test_vector2.cpp
    test_vector3.cpp
    test_layerstack.cpp
    test_logger.cpp

    test_aabbtree.cpp
    test_threadpool.cpp
    test_jobcontroller.cpp
    test_scheduler.cpp
    test_component.cpp
    test_ecswrapper.cpp
    test_commandbuffer.cpp
    test_inputhandler.cpp
)

target_link_libraries(EngineTests PRIVATE Engine Catch2::Catch2WithMain)

enable_testing()
add_test(NAME EngineTests COMMAND EngineTests)
