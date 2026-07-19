#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>
#include <glm/glm.hpp>

namespace Engine {
	class Camera : public Base {
	public:
		explicit Camera(const CameraDesc& desc);
		~Camera();

		void setPosition(const Vector2double& position) noexcept;
		[[nodiscard]] const Vector2double getPosition() const noexcept;

		void setZoom(f32 zoom) noexcept;

		[[nodiscard]] f32 getZoom() const noexcept;

		[[nodiscard]] glm::mat4 getProjection(i32 viewportWidth, i32 viewportHeight) const noexcept;

		[[nodiscard]] Vector2double toCameraRelative(const Vector2double& worldPosition) const noexcept;

	private:
		Vector2double m_position{};
		f32 m_zoom{ 1.0f };

	};
}