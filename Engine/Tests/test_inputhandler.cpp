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

// NOTE: InputHandler::onKey/onMouseButton/onMouseMove/onScroll are private with `friend class
// Application` (only ever driven by GLFW's callbacks via a real Application/Window). There is
// no public seam to simulate a keypress/mouse event from a test, so these tests are limited to
// the handler's default/no-input state and its error-handling paths for unbound ActionIDs,
// rather than press/release/move transitions.

TEST_CASE("ActionMap's default bindings include the expected gameplay actions", "[ActionMap]") {
    ActionMap actionMap{};
    auto hasBinding = [&](i32 key, ActionID action) {
        return std::any_of(actionMap.bindings.begin(), actionMap.bindings.end(),
            [&](const auto& pair) { return pair.first == key && pair.second == action; });
        };

    REQUIRE_FALSE(actionMap.bindings.empty());
    REQUIRE(hasBinding(GLFW_KEY_SPACE, ActionID::Pause));
    REQUIRE(hasBinding(GLFW_KEY_W, ActionID::PanUp));
    REQUIRE(hasBinding(GLFW_KEY_S, ActionID::PanDown));
    REQUIRE(hasBinding(GLFW_KEY_A, ActionID::PanLeft));
    REQUIRE(hasBinding(GLFW_KEY_D, ActionID::PanRight));
    REQUIRE(hasBinding(GLFW_KEY_UP, ActionID::ZoomOut));
    REQUIRE(hasBinding(GLFW_KEY_DOWN, ActionID::ZoomIn));
}

TEST_CASE_METHOD(InputHandlerFixture, "a freshly constructed InputHandler reports every key as up", "[InputHandler]") {
    REQUIRE_FALSE(input.isKeyDown(GLFW_KEY_SPACE));
    REQUIRE_FALSE(input.wasKeyJustPressed(GLFW_KEY_SPACE));
    REQUIRE_FALSE(input.wasKeyJustReleased(GLFW_KEY_SPACE));
}

TEST_CASE_METHOD(InputHandlerFixture, "isKeyDown with an out-of-range key returns false rather than indexing out of bounds", "[InputHandler]") {
    REQUIRE_FALSE(input.isKeyDown(-1));
    REQUIRE_FALSE(input.isKeyDown(GLFW_KEY_LAST + 1));
}

TEST_CASE_METHOD(InputHandlerFixture, "isKeyDown by ActionID resolves through ActionMap's bindings and reports false with no input yet", "[InputHandler]") {
    REQUIRE_FALSE(input.isKeyDown(ActionID::Pause));
}

TEST_CASE_METHOD(InputHandlerFixture, "wasEventActivated for a bound ActionID reports false with no input yet", "[InputHandler]") {
    REQUIRE_FALSE(input.wasEventActivated(ActionID::Pause));
}

TEST_CASE_METHOD(InputHandlerFixture, "wasEventActivated with an unbound ActionID logs an error and returns false", "[InputHandler]") {
    REQUIRE_FALSE(input.wasEventActivated(ActionID::Count));
}

TEST_CASE_METHOD(InputHandlerFixture, "isKeyDown with an unbound ActionID logs an error and returns false", "[InputHandler]") {
    REQUIRE_FALSE(input.isKeyDown(ActionID::Count));
}

TEST_CASE_METHOD(InputHandlerFixture, "a freshly constructed InputHandler reports every mouse button as up", "[InputHandler]") {
    REQUIRE_FALSE(input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT));
    REQUIRE_FALSE(input.wasMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT));
}

TEST_CASE_METHOD(InputHandlerFixture, "isMouseButtonDown with an out-of-range button returns false rather than indexing out of bounds", "[InputHandler]") {
    REQUIRE_FALSE(input.isMouseButtonDown(-1));
    REQUIRE_FALSE(input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LAST + 1));
}

TEST_CASE_METHOD(InputHandlerFixture, "a freshly constructed InputHandler reports zeroed mouse position, delta, and scroll", "[InputHandler]") {
    REQUIRE(input.getMousePosition().x == 0.0);
    REQUIRE(input.getMousePosition().y == 0.0);
    REQUIRE(input.getMouseDelta().x == 0.0);
    REQUIRE(input.getMouseDelta().y == 0.0);
    REQUIRE(input.getScrollDelta() == 0.0f);
}

TEST_CASE_METHOD(InputHandlerFixture, "endFrame is safe to call with no prior input", "[InputHandler]") {
    REQUIRE_NOTHROW(input.endFrame());
}
