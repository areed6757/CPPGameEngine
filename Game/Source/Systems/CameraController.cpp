#include <Systems/CameraController.h>

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
	if (m_inHandle.isKeyDown("panleft")) panDir.x -= 1.0;
	if (m_inHandle.isKeyDown("panright")) panDir.x += 1.0;
	if (m_inHandle.isKeyDown("pandown")) panDir.y -= 1.0;
	if (m_inHandle.isKeyDown("panup")) panDir.y += 1.0;

	// Direct comparison to a double is acceptable because 0.0 and 1.0 are mathematically perfectly represented in a double and no
	//	other values are ever possible for panDir
	if (panDir.x != 0.0 || panDir.y != 0.0) {
		panDir.normalize();
		f32 speed = m_basePanSpeed * m_camera.getZoom();
		m_camera.setPosition(m_camera.getPosition() + panDir * static_cast<d64>(speed * fdt));
	}
}
