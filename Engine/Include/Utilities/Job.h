#pragma once
#include <ECS/TickedSystem.h>
#include <bitset>
#include <functional>

namespace Engine {
	struct Job {
		std::bitset<64> reads{};
		std::bitset<64> writes{};
		std::function<void()> execute;
		TickedSystem* owner{ nullptr };
	};
}