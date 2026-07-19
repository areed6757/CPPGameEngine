#include <Graphics/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine { 
	Engine::Camera::Camera(const CameraDesc& desc) : Base(desc.base),
		m_position(desc.position),
		m_zoom(desc.zoom)
	{
		EngineLogInfo("Camera created.");
	}

	Engine::Camera::~Camera()
	{
		EngineLogInfo("Camera destroyed.");
	}

	void Engine::Camera::setPosition(const Vector2double& position) noexcept
	{
		m_position = position;
	}

	const Vector2double Engine::Camera::getPosition() const noexcept
	{
		return m_position;
	}

	void Engine::Camera::setZoom(f32 zoom) noexcept
	{
		m_zoom = zoom;
	}

	f32 Engine::Camera::getZoom() const noexcept
	{
		return m_zoom;
	}

	glm::mat4 Engine::Camera::getProjection(i32 viewportWidth, i32 viewportHeight) const noexcept
	{
		f32 aspect = static_cast<f32>(viewportWidth) / static_cast<f32>(viewportHeight);
		f32 halfHeight = m_zoom;
		f32 halfWidth = halfHeight * aspect;
		return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -100.0f, 100.0f);
	}

	Vector2double Engine::Camera::toCameraRelative(const Vector2double& worldPosition) const noexcept
	{
		return worldPosition - m_position;
	}
}