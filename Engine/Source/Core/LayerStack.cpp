#include <Core/LayerStack.h>
#include <Core/Layer.h>
#include <algorithm>

namespace Engine {
	LayerStack::LayerStack(const LayerStackDesc& desc) : Base(desc.base) {

	}

	LayerStack::~LayerStack() {
		for (Layer* layer : m_layers) {
			layer->onDetach();
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer) {
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex++, layer);
		layer->onAttach();
		EngineLogInfo("LayerStack: pushed layer");
	}

	void LayerStack::pushOverlay(Layer* layer) {
		m_layers.emplace_back(layer);
		layer->onAttach();
		EngineLogInfo("LayerStack: pushed overlay");
	}

	void LayerStack::popLayer(Layer* layer) {
		auto it = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, layer);
		if (it != m_layers.begin() + m_layerInsertIndex) {
			layer->onDetach();
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(Layer* layer) {
		auto it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), layer);
		if (it != m_layers.end()) {
			layer->onDetach();
			m_layers.erase(it);
		}
	}
}