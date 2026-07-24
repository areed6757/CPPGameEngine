#pragma once
#include <Core/Common.h>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace Engine {
	class ThreadPool : public Base {
	public:
		explicit ThreadPool(const ThreadPoolDesc& desc);
		~ThreadPool();

		void submit(std::function<void()> task);
		void waitForAll();

		i32 threadCount() const noexcept { return static_cast<i32>(m_workers.size()); }

	private:
		void workerLoop();

		std::vector<std::thread> m_workers;
		std::queue<std::function<void()>> m_tasks;

		std::mutex m_queueMutex;
		std::condition_variable m_queueCondition;

		std::mutex m_doneMutex;
		std::condition_variable m_doneCondition;
		std::atomic<i32> m_inFlight{ 0 };

		bool m_shuttingDown{ false };
	};
}