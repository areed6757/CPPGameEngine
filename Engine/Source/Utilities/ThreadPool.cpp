#include <Utilities/ThreadPool.h>

namespace Engine {
	ThreadPool::ThreadPool(const ThreadPoolDesc& desc) : Base(desc.base)
	{
		i32 count = desc.threadCount > 0 ? desc.threadCount : 1;
		m_workers.reserve(count);
		for (i32 i = 0; i < count; i++) {
			m_workers.emplace_back(&ThreadPool::workerLoop, this);
		}
		EngineLogInfo("Thread pool created with {} workers threads.", count);
	}

	ThreadPool::~ThreadPool() {
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_shuttingDown = true;
		}
		m_queueCondition.notify_all();
		for (auto& worker : m_workers) {
			if (worker.joinable()) { worker.join(); }
		}
		EngineLogInfo("Thread pool destroyed.");
	}

	void ThreadPool::submit(std::function<void()> task) {
		m_inFlight.fetch_add(1, std::memory_order_relaxed);
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_tasks.push(std::move(task));
		}
		m_queueCondition.notify_one();
	}

	void ThreadPool::waitForAll() {
		std::unique_lock<std::mutex> lock(m_doneMutex);
		m_doneCondition.wait(lock, [this]() { return m_inFlight.load(std::memory_order_acquire) == 0; });
	}

	void ThreadPool::workerLoop() {
		while (true) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_queueCondition.wait(lock, [this]() {return m_shuttingDown || !m_tasks.empty(); });

				if (m_shuttingDown && m_tasks.empty()) { return; }

				task = std::move(m_tasks.front());
				m_tasks.pop();
			}

			task();

			if (m_inFlight.fetch_sub(1, std::memory_order_acq_rel) == 1) {
				std::lock_guard<std::mutex> lock(m_doneMutex);
				m_doneCondition.notify_all();
			}
		}
	}
}