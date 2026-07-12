#pragma once
#include <Core/Common.h>
#include <ECS/Component.h>
#include <ECS/Components/Transform.h>

namespace Engine {
	struct ComponentPools {
		Component<Transform> transforms;
	};

	class ECSWrapper : public Base {
	public:
		explicit ECSWrapper(const ECSWrapperDesc& desc);
		~ECSWrapper();

	private:
		EntityRegister& m_entityReg;
	};
}