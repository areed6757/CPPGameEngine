#include <GUI/ImGUILayer.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Engine {
	ImGUILayer::ImGUILayer(Application& app) : Base({app.getLogger()}), Layer("ImGuiLayer"), m_app(app) {}

	void ImGUILayer::onAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui::GetIO().FontGlobalScale = 1.5f;
		ImGui::GetIO().DisplayFramebufferScale;

		// Setting arg2, install_callbacks, saves whatever callback is on the window already
		// and chains to it after processing input itself
		// ImGUILayer must be pushed after GameLayer for this to function properly
		ImGui_ImplGlfw_InitForOpenGL(m_app.getWindow().get(), true);
		ImGui_ImplOpenGL3_Init("#version 440");

		EngineLogInfo("ImGuiLayer attached.");
	}

	ImGUILayer::~ImGUILayer() {
		
	}

	void ImGUILayer::onDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		EngineLogInfo("ImGuiLayer detached.");
	}

	void ImGUILayer::onUpdate(d64 dt)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug Overlay");
		ImGui::Text("Frame time: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		// Tick time, entity count, particle count, need getters and counts for these

		for (auto& panel : m_panels) { panel(); }

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGUILayer::addPanel(std::function<void()> drawFn)
	{
		m_panels.push_back(std::move(drawFn));
	}

}