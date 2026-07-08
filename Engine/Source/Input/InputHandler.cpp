#include <Input/InputHandler.h>

Engine::InputHandler::InputHandler(InputHandlerDesc& desc) : Base(desc.base), m_bindings(desc.actionBindings)
{

}

Engine::InputHandler::~InputHandler()
{
}

void Engine::InputHandler::onKey(i32 key, i32 scancode, i32 action, i32 mods)
{
}
