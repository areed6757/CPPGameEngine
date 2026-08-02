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
		i32 current = static_cast<i32>(m_editCategory);
		if (ImGui::Combo("Category", &current, categoryNames, IM_ARRAYSIZE(categoryNames))) {
			m_editCategory = static_cast<PartCategory>(current);
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
			break;
		}
		case PartCategory::Armor: {
			ImGui::DragFloat("Mass", &m_armorParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_armorParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_armorParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_armorParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			break;
		}
		case PartCategory::Engine: {
			ImGui::DragFloat("Mass", &m_engineParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_engineParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_engineParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_engineParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Thrust Force", &m_engineParams.thrustForce, 1.0f, 0.0f, 10000.0f);
			ImGui::DragFloat("Power Draw", &m_engineParams.powerDraw, 0.5f, 0.0f, 1000.0f);
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
			break;
		}
		case PartCategory::Hardpoint: {
			ImGui::DragFloat("Mass", &m_hardpointParams.mass, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Health", &m_hardpointParams.health, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Stability Contribution", &m_hardpointParams.stabilityContribution, 0.5f, 0.0f, 1000.0f);
			ImGui::DragFloat("Kinetic Resistance", &m_hardpointParams.kineticResistance, 0.01f, 0.0f, 1.0f);
			ImGui::SliderInt("Size X", &m_hardpointParams.sizeX, 1, 4);
			ImGui::SliderInt("Size Y", &m_hardpointParams.sizeY, 1, 4);
			break;
		}
		default: break;
		}
	}

	void PartEditorLayer::save()
	{
		PartVariant variant;
		variant.name = m_editName.substr(0, m_editName.find('\0'));
		variant.category = m_editCategory;

		switch (m_editCategory) {
		case PartCategory::Hull: variant.params = m_hullParams; break;
		case PartCategory::Armor: variant.params = m_armorParams; break;
		case PartCategory::Engine: variant.params = m_engineParams; break;
		case PartCategory::Weapon: variant.params = m_weaponParams; break;
		case PartCategory::Hardpoint: variant.params = m_hardpointParams; break;
		default: return;
		}

		std::filesystem::path outPath = std::filesystem::path("parts") / (m_editName + ".json");
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
		ImGui::Begin("Part Editor");

		ImGui::InputText("Name", m_editName.data(), m_editName.capacity() + 1);
		drawCategoryPicker();
		ImGui::Separator();
		drawStatFields();
		ImGui::Separator();

		if (ImGui::Button("Save")) { save(); }
		if (!m_saveStatus.empty()) { ImGui::TextUnformatted(m_saveStatus.c_str()); }

		ImGui::End();
	}
}