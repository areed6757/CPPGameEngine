#include <Input/InputHandler.h>
#include <format>

Engine::InputHandler::InputHandler(InputHandlerDesc& desc) : Base(desc.base), m_actionMap(desc.actionMap), m_keyStates(GLFW_KEY_LAST + 1, false)
{
	m_actionMap.bindings.shrink_to_fit();
}

Engine::InputHandler::~InputHandler()
{
}

void Engine::InputHandler::onKey(i32 key, i32 scancode, i32 action, i32 mods)
{	
	switch (action) {
	case(GLFW_PRESS):
		for (const std::pair pairs : m_actionMap.bindings) {
			if (pairs.first == key) {
				// Later : Send events for key presses.
				m_keyStates.at(key) = true;
				EngineLogInfo((std::format("{} pressed", pairs.second)).c_str());
			}
		}
		return;
	case(GLFW_RELEASE):
		for (const std::pair pairs : m_actionMap.bindings) {
			if (pairs.first == key) {
				// Later : Send events for key presses.
				m_keyStates.at(key) = false;
				EngineLogInfo((std::format("{} released", pairs.second)).c_str());
			}
		}
		return;
	case(GLFW_REPEAT):
		// Handle key repeats (LOOK THIS UP IN GLFW DOCS)
		EngineLogWarning("GLFW_REPEAT key action called but not defined in InputHandler.");
		return;
	default:
		EngineLogWarning("Unexpected GLFW key action.");
		return;
	}
}

bool Engine::InputHandler::isKeyDown(i32 key) const noexcept
{
	return m_keyStates[key];
}
