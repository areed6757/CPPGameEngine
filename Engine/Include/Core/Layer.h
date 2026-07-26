#pragma once
#include <string>
#include <Core/Common.h>

namespace Engine {
	class Layer {
	public:
		explicit Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;
		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(d64 dt) {}
	protected:
		std::string m_debugName;
	};
}