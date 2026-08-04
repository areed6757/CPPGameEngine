#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>
#include <Physics/AABBTree.h>
#include <glm/glm.hpp>

namespace Engine {
	struct CameraDesc {
		BaseDesc base;
		Vector2double position{ 0.0, 0.0 };
		f32 zoom{ 10.0f };
	};

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

		[[nodiscard]] AABB getViewportBounds(i32 viewportWidth, i32 viewportHeight) const noexcept;

		[[nodiscard]] f32 getWorldUnitsPerPixel(i32 viewportHeight) const noexcept {
			return (2.0f * m_zoom) / static_cast<f32>(viewportHeight);
		}

	private:
		Vector2double m_position{};
		f32 m_zoom{ 1.0f };

	};
}