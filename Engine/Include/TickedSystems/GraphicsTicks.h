#pragma once
#include <Core/Common.h>
#include <TickedSystems/TickedSystem.h>

// TODO: THIS CLASS IS FOR TESTING. IT WILL HANDLE GRAPHICS UPDATES LATER
namespace Engine{
	class GraphicsTicks final : public Base, public TickedSystem {
	public :
		explicit GraphicsTicks(GraphicsTicksDesc& desc);
		~GraphicsTicks();

		void Update(d64 deltaTime) override;

	private:
		i32 m_tickCount = 0;
	};
}