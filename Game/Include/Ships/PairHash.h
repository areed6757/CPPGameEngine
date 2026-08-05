#pragma once
#include <Core/Common.h>
#include <Ships/PartVariant.h>
#include <unordered_map>
#include <utility>

namespace Engine {
	struct PairHash {
		std::size_t operator()(const std::pair<i32, i32>& p) const noexcept {
			uint64_t combined = (static_cast<uint64_t>(static_cast<uint32_t>(p.first)) << 32) | static_cast<uint32_t>(p.second);
			return std::hash<uint64_t>{}(combined);
		}
	};

	using HardpointLoadout = std::unordered_map<std::pair<i32, i32>, PartVariantID, PairHash>;
}