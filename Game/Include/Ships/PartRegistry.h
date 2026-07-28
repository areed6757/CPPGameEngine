#pragma once
#include <Game.h>
#include <vector>

namespace Engine {
	struct PartRegistryDesc {
		BaseDesc base;
	};

	class PartRegistry : public Base {
	public:
		explicit PartRegistry(const PartRegistryDesc& desc);
		~PartRegistry();

		PartVariantID registerVariant(const PartVariant& variant);
		[[nodiscard]] const PartVariant& get(PartVariantID id) const;
		[[nodiscard]] i32 size() const noexcept { return static_cast<i32>(m_variants.size()); }

	private:
		std::vector<PartVariant> m_variants;
	};
}