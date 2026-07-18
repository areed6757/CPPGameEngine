#pragma once

namespace Engine {
	// Order currently matters for these, new entries should be added after <Triangle>
	enum class MeshID {
		Quad,
		Triangle,
		Count // Must remain last
		// Hull archetypes?
	};
}