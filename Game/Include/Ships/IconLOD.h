#pragma once
#include <Graphics/TextureID.h>
#include <Core/Common.h>

namespace Engine {
	struct ShipIconTier {
		f32 minPixelSize;
		TextureID texture;
	};

	// Runtime-tunable icon LOD thresholds, adjustable live via the "Icon LOD" debug panel
	// (GameLayer). Ship tiers are indexed by PartRenderSystem::tierForPartCount; the pixel
	// thresholds control when a ship swaps to its icon, texture controls which icon is used.
	inline ShipIconTier g_shipIconTiers[]{
		{ 14.0f, TextureID::FighterIcon },
		{ 20.0f, TextureID::FighterIcon },
		{ 30.0f, TextureID::FrigateIcon },
		{ 45.0f, TextureID::DestroyerIcon }
	};

	// Applied to newly-fired projectiles by WeaponSystem; changing this at runtime only affects subsequent projectiles spawned
	inline f32 g_projectileIconMinPixelSize = 10.0f;
	inline TextureID g_projectileIconTexture = TextureID::ProjectileIcon;

	constexpr f32 ICON_ART_ROTATION_OFFSET = -HALF_PI;
}
