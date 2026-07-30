#include <Systems/CameraController.h>
#include <algorithm>

Engine::CameraController::CameraController(const CameraControllerDesc& desc) : Base(desc.base),
	m_camera(desc.camera),
	m_inHandle(desc.inputHandler)
{
}

Engine::CameraController::~CameraController()
{
}

void Engine::CameraController::Update(d64 dt)
{
	f32 fdt = static_cast<f32>(dt);
	Vector2double panDir{};
	if (m_inHandle.isKeyDown(ActionID::PanLeft)) panDir.x -= 1.0;
	if (m_inHandle.isKeyDown(ActionID::PanRight)) panDir.x += 1.0;
	if (m_inHandle.isKeyDown(ActionID::PanDown)) panDir.y -= 1.0;
	if (m_inHandle.isKeyDown(ActionID::PanUp)) panDir.y += 1.0;

	// Direct comparison to a double is acceptable because 0.0 and 1.0 are mathematically perfectly represented in a double and no
	//	other values are ever possible for panDir
	if (panDir.x != 0.0 || panDir.y != 0.0) {
		panDir.normalize();
		f32 speed = m_basePanSpeed * m_camera.getZoom();
		m_camera.setPosition(m_camera.getPosition() + panDir * static_cast<d64>(speed * fdt));
	}

	// Zoom in and out

	if (m_inHandle.isKeyDown(ActionID::ZoomIn)) {
		m_camera.setZoom(std::clamp(m_camera.getZoom() * std::pow(m_zoomFactor, -fdt), m_minZoom, m_maxZoom));
	}	
	
	if (m_inHandle.isKeyDown(ActionID::ZoomOut)) {
		m_camera.setZoom(std::clamp(m_camera.getZoom() * std::pow(m_zoomFactor, fdt), m_minZoom, m_maxZoom));
	}
}
