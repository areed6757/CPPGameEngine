#include <PartEditorLayer.h>
#include <imgui.h>

namespace Engine {
	PartEditorLayer::PartEditorLayer(Application& app, PartRegistry& partReg) :
		Base({ app.getLogger() }),
		m_app(app),
		m_partRegistry(partReg)
	{
		m_editName.resize(64, '\0');
	}

	void PartEditorLayer::drawCategoryPicker()
	{
		static const char* categoryNames[] = { "Hull", "Armor", "Engine", "Weapon", "Hardpoint" };
		i32 current = static_cast<i32>(m_editCategory) - 1; // categoryNames[0] corresponds to Hull (value 1) not None (value 0)
		if (ImGui::Combo("Category", &current, categoryNames, IM_ARRAYSIZE(categoryNames))) {
			m_editCategory = static_cast<PartCategory>(current +1);
		}
	}

	void PartEditorLayer::drawWeaponRolePicker()
	{
		static const char* roleNames[] = {
			"Point Defense", "Flak", "Light Primary", "Standard Primary",
			"Rapid Primary", "Heavy Primary", "Siege Primary", "Torpedo"
		};
		i32 current = static_cast<i32>(m_weaponParams.role);
		if (ImGui::Combo("Role", &current, roleNames, IM_ARRAYSIZE(roleNames))) {
			m_weaponParams.role = static_cast<WeaponRole>(current);
		}
	}

	void PartEditorLayer::drawStatFields()
	{
		switch (m_editCategory) {
		case PartCategory::Hull: {
			ImGui::DragFloat("Mass", &m_hullParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_hullParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_hullParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_hullParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("System Capacity", &m_hullParams.systemCapacityContribution, 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("Signal Emission", &m_hullParams.signalEmissionValue, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Sensor Power", &m_hullParams.sensorPowerValue, 0.05f, 0.0f, 10.0f);
			break;
		}
		case PartCategory::Armor: {
			ImGui::DragFloat("Mass", &m_armorParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_armorParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_armorParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_armorParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("System Capacity", &m_armorParams.systemCapacityContribution, 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("Signal Emission", &m_armorParams.signalEmissionValue, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Sensor Power", &m_armorParams.sensorPowerValue, 0.05f, 0.0f, 10.0f);
			break;
		}
		case PartCategory::Engine: {
			ImGui::DragFloat("Mass", &m_engineParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_engineParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_engineParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_engineParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Thrust Force", &m_engineParams.thrustForce, 1.0f, 0.0f, 10000.0f);
			ImGui::DragFloat("Power Draw", &m_engineParams.powerDraw, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("System Capacity", &m_engineParams.systemCapacityContribution, 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("Signal Emission", &m_engineParams.signalEmissionValue, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Sensor Power", &m_engineParams.sensorPowerValue, 0.05f, 0.0f, 10.0f);
			break;
		}
		case PartCategory::Weapon: {
			ImGui::DragFloat("Mass", &m_weaponParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_weaponParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_weaponParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_weaponParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Damage", &m_weaponParams.damage, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Cooldown", &m_weaponParams.cooldown, 0.05f, 0.01f, 60.0f);
			ImGui::DragFloat("Projectile Speed", &m_weaponParams.projectileSpeed, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Projectile Radius", &m_weaponParams.projectileRadius, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Projectile Lifetime", &m_weaponParams.projectileLifetime, 0.05f, 0.01f, 60.0f);
			ImGui::SliderInt("Barrel Count", &m_weaponParams.barrelCount, 1, 4);
			ImGui::DragFloat("Barrel Spacing", &m_weaponParams.barrelSpread, 0.001f, 0.0f, 0.25f);
			ImGui::DragFloat("Muzzle Forward Offset", &m_weaponParams.muzzleForwardOffset, 0.001f, 0.0f, 0.25f);
			ImGui::DragFloat("System Capacity", &m_weaponParams.systemCapacityContribution, 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("Signal Emission", &m_weaponParams.signalEmissionValue, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Sensor Power", &m_weaponParams.sensorPowerValue, 0.05f, 0.0f, 10.0f);
			drawWeaponRolePicker();
			break;
		}
		case PartCategory::Hardpoint: {
			ImGui::DragFloat("Mass", &m_hardpointParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_hardpointParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_hardpointParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_hardpointParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::SliderInt("Size X", &m_hardpointParams.sizeX, 1, 4);
			ImGui::SliderInt("Size Y", &m_hardpointParams.sizeY, 1, 4);
			ImGui::DragFloat("System Capacity", &m_hardpointParams.systemCapacityContribution, 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("Signal Emission", &m_hardpointParams.signalEmissionValue, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Sensor Power", &m_hardpointParams.sensorPowerValue, 0.05f, 0.0f, 10.0f);
			break;
		}
		default: break;
		}
	}

	void PartEditorLayer::save()
	{
		std::string trimmedName = m_editName.substr(0, m_editName.find('\0'));

		PartVariant variant;
		variant.name = trimmedName;
		variant.category = m_editCategory;

		switch (m_editCategory) {
		case PartCategory::Hull: variant.params = m_hullParams; break;
		case PartCategory::Armor: variant.params = m_armorParams; break;
		case PartCategory::Engine: variant.params = m_engineParams; break;
		case PartCategory::Weapon: variant.params = m_weaponParams; break;
		case PartCategory::Hardpoint: variant.params = m_hardpointParams; break;
		default: return;
		}

		std::filesystem::path outPath = std::filesystem::path(SOURCE_ROOT) / "Assets" / "Parts" / (trimmedName + ".json");
		std::filesystem::create_directories(outPath.parent_path());

		if (savePartVariant(variant, outPath)) {
			m_partRegistry.registerVariant(variant);
			m_saveStatus = "Saved: " + outPath.string();
		}
		else {
			m_saveStatus = "Save FAILED: " + outPath.string();
		}
	}

	void PartEditorLayer::draw()
	{
		ImGui::InputText("Name", m_editName.data(), m_editName.capacity() + 1);
		drawCategoryPicker();
		ImGui::Separator();
		drawStatFields();
		ImGui::Separator();

		if (ImGui::Button("Save")) { save(); }
		if (!m_saveStatus.empty()) { ImGui::TextUnformatted(m_saveStatus.c_str()); }
	}
}