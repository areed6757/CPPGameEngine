#pragma once
#include <Core/Base.h>
#include <Ships/PartRegistry.h>
#include <Ships/PartVariant.h>
#include <Serialization/PartVariantSerializer.h>
#include <string>

namespace Engine {
	struct PartEditorLayerDesc {
		BaseDesc base;
		PartRegistry& partRegistry;
	};

	class PartEditorLayer : public Base {
	public:
		PartEditorLayer(Application& app, PartRegistry& partReg);

		void draw();

	private:
		void drawCategoryPicker();
		void drawStatFields();
		void drawWeaponRolePicker();
		void save();

		Application& m_app;
		PartRegistry& m_partRegistry;

		std::string m_editName{ "New Part" };
		PartCategory m_editCategory{ PartCategory::Hull };
		HullParams m_hullParams{};
		ArmorParams m_armorParams{};
		EngineParams m_engineParams{};
		WeaponParams m_weaponParams{};
		HardpointParams m_hardpointParams{};

		std::string m_saveStatus;
	};
}