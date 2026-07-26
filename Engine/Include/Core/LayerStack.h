#pragma once
#include <Core/Common.h>
#include <vector>

namespace Engine {
	struct LayerStackDesc {
		BaseDesc base;
	};

	class Layer;

	class LayerStack : public Base {
	public:
		explicit LayerStack(const LayerStackDesc& desc);
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }

	private:
		std::vector<Layer*> m_layers;
		i32 m_layerInsertIndex{ 0 };
	};
}