#include <Graphics/Camera.h>

Engine::Camera::Camera(const CameraDesc& desc)
{
}

Engine::Camera::~Camera()
{
}

void Engine::Camera::setPosition(const Vector2double& position) noexcept
{
}

const Vector2double Engine::Camera::getPosition() const noexcept
{
	return Vector2double();
}

void Engine::Camera::setZoom(f32 zoom) noexcept
{
}

f32 Engine::Camera::getZoom() const noexcept
{
	return f32();
}

glm::mat4 Engine::Camera::getProjection(i32 viewportWidth, i32 viewportHeight) const noexcept
{
	return glm::mat4();
}

Vector2double Engine::Camera::toCameraRelative(const Vector2double& worldPosition) const noexcept
{
	return Vector2double();
}
