#pragma once
#include <Ships/PartTypes.h>
#include <Ships/PartVariant.h>
#include <Physics/Vector2float.h>
#include <Core/Common.h>
#include <vector>

namespace Engine {
	struct PartVisualInstance {
		Vector2float localOffset;
		i32 sizeX, sizeY;
		PartCategory category;
		PartVariantID variant;
	};

	struct ShipVisual {
		std::vector<PartVisualInstance> parts;
	};
}