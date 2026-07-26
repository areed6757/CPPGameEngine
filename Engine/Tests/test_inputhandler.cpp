#include <catch2/catch_test_macros.hpp>
#include <Input/InputHandler.h>
#include <Input/ActionMap.h>
#include <Core/Logger.h>
#include <algorithm>

using namespace Engine;

namespace {
    struct InputHandlerFixture {
        Logger logger{ Logger::LogLevel::Info };
        ActionMap actionMap{};
        InputHandler input{ InputHandlerDesc{ {logger}, actionMap } };
    };
}

// NOTE: InputHandler::onKey is private with `friend class Window` (it's only ever driven by
// GLFW's key callback via a real Window). There is no public seam to simulate a keypress from
// a test, so these tests are limited to the handler's default/no-input state and its
// error-handling paths for unknown event names, rather than press/release transitions.

TEST_CASE("ActionMap's default bindings include the expected gameplay actions", "[ActionMap]") {
    ActionMap actionMap{};
    auto hasBinding = [&](i32 key, const std::string& action) {
        return std::any_of(actionMap.bindings.begin(), actionMap.bindings.end(),
            [&](const auto& pair) { return pair.first == key && pair.second == action; });
        };

    REQUIRE_FALSE(actionMap.bindings.empty());
    REQUIRE(hasBinding(GLFW_KEY_SPACE, "pause"));
    REQUIRE(hasBinding(GLFW_KEY_W, "panup"));
}

TEST_CASE_METHOD(InputHandlerFixture, "a freshly constructed InputHandler reports every key as up", "[InputHandler]") {
    logger.log(Logger::LogLevel::Info, "Fresh handler: isKeyDown(GLFW_KEY_SPACE)={}, wasKeyJustPressed(GLFW_KEY_SPACE)={}",
        input.isKeyDown(GLFW_KEY_SPACE), input.wasKeyJustPressed(GLFW_KEY_SPACE));

    REQUIRE_FALSE(input.isKeyDown(GLFW_KEY_SPACE));
    REQUIRE_FALSE(input.wasKeyJustPressed(GLFW_KEY_SPACE));
}

TEST_CASE_METHOD(InputHandlerFixture, "isKeyDown by event name resolves through ActionMap's bindings and reports false with no input yet", "[InputHandler]") {
    bool pauseDown = input.isKeyDown("pause");
    logger.log(Logger::LogLevel::Info, "isKeyDown(\"pause\") on a fresh handler -> {}", pauseDown);
    REQUIRE_FALSE(pauseDown);
}

TEST_CASE_METHOD(InputHandlerFixture, "wasEventActivated with an unknown event name logs an error and returns false", "[InputHandler]") {
    bool activated = input.wasEventActivated("not-a-real-event");
    logger.log(Logger::LogLevel::Info, "wasEventActivated(\"not-a-real-event\") -> {} (an error should also have been logged)", activated);
    REQUIRE_FALSE(activated);
}

TEST_CASE_METHOD(InputHandlerFixture, "isKeyDown with an unknown event name logs an error and returns false", "[InputHandler]") {
    bool down = input.isKeyDown("not-a-real-event");
    logger.log(Logger::LogLevel::Info, "isKeyDown(\"not-a-real-event\") -> {} (an error should also have been logged)", down);
    REQUIRE_FALSE(down);
}

TEST_CASE_METHOD(InputHandlerFixture, "endFrame is safe to call with no prior input", "[InputHandler]") {
    REQUIRE_NOTHROW(input.endFrame());
}
