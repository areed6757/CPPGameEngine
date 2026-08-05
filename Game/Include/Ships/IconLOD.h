#pragma once
#include <Graphics/TextureID.h>
#include <Core/Common.h>

namespace Engine {
	struct ShipIconTier {
		f32 swapThreshold;
		f32 iconPixelSize;
		TextureID texture;
	};

	// Indexed by tierForPartCount. Both fields runtime-tunable via the "Icon LOD" debug panel (GameLayer)
	inline ShipIconTier g_shipIconTiers[]{
		{ 80.0f, 8.0f, TextureID::FighterIcon },
		{ 75.0f, 12.0f, TextureID::FighterIcon },
		{ 70.0f, 16.0f, TextureID::FrigateIcon },
		{ 65.0f, 24.0f, TextureID::DestroyerIcon }
	};

	inline f32 g_projectileIconMinPixelSize = 6.0f;
	inline TextureID g_projectileIconTexture = TextureID::ProjectileIcon;

	constexpr f32 ICON_ART_ROTATION_OFFSET = -HALF_PI;

	inline i32 tierForPartCount(i32 partCount) {
		if (partCount < 10) return 0;
		if (partCount < 30) return 1;
		if (partCount < 60) return 2; // frigate
		return 3; // destroyer, up to ~100 parts
	}

	// shared by PartRenderSystem and RenderSystem, so mount/turret entities agree with their owner ship's icon LOD
	inline bool shouldRenderAsIcon(i32 partCount, f32 shipRadius, f32 worldUnitsPerPixel) {
		const ShipIconTier& tier = g_shipIconTiers[tierForPartCount(partCount)];
		f32 shipPixelDiameter = (shipRadius * 2.0f) / worldUnitsPerPixel;
		return shipPixelDiameter < tier.swapThreshold;
	}
}
