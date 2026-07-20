#include <Systems/QuadtreeDebugSystem.h>


namespace Engine {
    QuadtreeDebugSystem::QuadtreeDebugSystem(const QuadtreeDebugSystemDesc& desc) : Base(desc.base),
        m_quadtree(desc.quadtree),
        m_camera(desc.camera),
        m_window(desc.window),
        m_debugLineRenderer(desc.debugLineRenderer)
	{

	}

    void QuadtreeDebugSystem::Update(d64 dt) {
        m_boundVerts.clear();
        m_quadtree.debugCollectBounds(m_boundVerts);

        m_flatVerts.clear();
        for (const auto& v : m_boundVerts) {
            Vector2double rel = m_camera.toCameraRelative(v);
            m_flatVerts.push_back(static_cast<f32>(rel.x));
            m_flatVerts.push_back(static_cast<f32>(rel.y));
            m_flatVerts.push_back(0.0f);
        }

        m_debugLineRenderer.draw(m_flatVerts, m_camera.getProjection(m_window.getWidth(), m_window.getHeight()), glm::vec3(0.0f, 1.0f, 0.0f));
    }
}