#pragma once
#include <Core/Common.h>

namespace Engine {
	// current broadcast value, seeded from BakedShipStats::totalSignalEmission at bake time but free to drift from it later, stealth/heat/active-sensor use will modify this independently
	struct SignalSignature {
		f32 magnitude{ 0.0f };
	};
}
