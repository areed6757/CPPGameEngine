#pragma once
#include <Engine.h>

namespace Engine {
	class ImGUILayer : public Base, public Layer {
	public:
		explicit ImGUILayer(Application& app);
		~ImGUILayer();
		
		void onAttach() override;
		void onDetach() override;
		void onUpdate(d64 dt) override;

	private:
		Application& m_app;
		
	};
}