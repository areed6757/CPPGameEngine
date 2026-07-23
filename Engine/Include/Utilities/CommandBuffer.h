#pragma once
#include <functional>
#include <Core/Common.h>
#include <ECS/ECSWrapper.h>
#include <ECS/EntityRegister.h>


/// <summary>
/// Extends ECS functions to create and destroy entities and components so that they may be queued for the end of a tick,
/// preventing concurrent threads from attempting to read/write improperly from an altered dense component/entity index
/// </summary>
namespace Engine {
	template <typename TPools>
	class CommandBuffer : public Base {
	public:
		explicit CommandBuffer(const CommandBufferDesc<TPools>& desc) : Base(desc.base), m_ecs(desc.ecs) {}

		[[nodiscard]] EntityID createEntity() { return m_ecs.createEntity(); }

		template <typename T>
		void addComponent(EntityID id, const T& component) {
			m_commands.emplace_back([id, component](ECSWrapper<TPools>& ecs) {
				ecs.template addComponent<T>(id, component);
				});
		}

		template <typename T>
		void removeComponent(EntityID id) {
			m_commands.emplace_back([id](ECSWrapper<TPools>& ecs) {
				ecs.template removeComponent<T>(id);
				});
		}

		void destroyEntity(EntityID id) {
			m_commands.emplace_back([id](ECSWrapper<TPools>& ecs) {ecs.destroyEntity(id); });
		}

		void flush() {
			for (auto& cmd : m_commands) { cmd(m_ecs); }
			m_commands.clear();
		}

	private:
		ECSWrapper<TPools>& m_ecs;
		std::vector<std::function<void(ECSWrapper<TPools>&)>> m_commands;
	};
}