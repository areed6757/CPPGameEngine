#pragma once
#include <Engine.h>

namespace Engine {
	// Debug Overlay is latched to the top-right corner of the main viewport on first use each
	// run (ImGuiCond_FirstUseEver), so tool windows don't need re-dragging after every rebuild.
	// Editor (GameLayer) anchors itself directly below using these same constants.
	constexpr float DEBUG_OVERLAY_WIDTH = 380.0f;
	constexpr float DEBUG_OVERLAY_HEIGHT = 90.0f;

	class ImGUILayer : public Base, public Layer {
	public:
		explicit ImGUILayer(Application& app);
		~ImGUILayer();
		
		void onAttach() override;
		void onDetach() override;
		void onUpdate(d64 dt) override;

		void addPanel(std::function<void()> drawFn);

	private:
		Application& m_app;
		std::vector<std::function<void()>> m_panels;
	};
}