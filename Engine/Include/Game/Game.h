#pragma once
#include <Core/Base.h>
#include <Core/Core.h>
#include <Graphics/GLFWContext.h>

namespace Engine {
	class Game : public Base {
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		virtual void run() final;


		// IMPORTANT: unique_ptrs must be instantiated in lowest to highest priority order.
		// The first of these objects created will be destroyed LAST.
	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GLFWContext> m_glfwContext{};
		bool m_isRunning{ true };
	};
}