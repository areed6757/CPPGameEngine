#pragma once
#include <Game.h>
#include <bitset>

namespace Engine {
	struct CameraControllerDesc {
		BaseDesc base;
		Camera& camera;
		InputHandler& inputHandler;
	};

	class CameraController : public Base, public TickedSystem {
	public:
		explicit CameraController(const CameraControllerDesc& desc);
		~CameraController();

		void Update(d64 dt) override;

	private:
		Camera& m_camera;
		InputHandler& m_inHandle;
		f32 m_basePanSpeed{ 3.0f };
		f32 m_zoomFactor{ 4.0f };
		f32 m_minZoom{ 0.5f };
		f32 m_maxZoom{ 200.0f };
	};
}