#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		Material& sphere1 = m_Scene.Materials.emplace_back();
		sphere1.Albedo = { 1.0f, 0.0f, 1.0f };
		sphere1.Roughness = 0.0f;

		Material& sphere2 = m_Scene.Materials.emplace_back();
		sphere2.Albedo = { 0.2f, 0.3f, 1.0f };
		sphere1.Roughness = 0.1f;

		Material& sphere3 = m_Scene.Materials.emplace_back();
		sphere1.Albedo = { 1.0f, 0.0f, 1.0f };
		sphere1.Roughness = 0.0f;

		Material& sphere4 = m_Scene.Materials.emplace_back();
		sphere2.Albedo = { 0.2f, 0.3f, 1.0f };
		sphere1.Roughness = 0.1f;

		Material& sphere5 = m_Scene.Materials.emplace_back();
		sphere2.Albedo = { 0.2f, 0.3f, 1.0f };
		sphere1.Roughness = 0.1f;


		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, -5.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 3.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 6.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 3;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 9.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 4;
			m_Scene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{

		ImGui::Begin("Info");

		if (m_ViewportWidth)
			ImGui::Text("Resolution : %dx%d", m_ViewportWidth, m_ViewportHeight);
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);\

		ImGui::End();



		ImGui::Begin("Settings");

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset Frame Index :"))
			m_Renderer.ResetFrameIndex();

		ImGui::Separator();

		if (ImGui::DragInt("Ray bounces nb", &m_Renderer.m_settings.bounces, 1, 0, 10000000))
			m_Renderer.ResetFrameIndex();
		ImGui::Separator();

		ImGui::End();



		ImGui::Begin("Scene");
		ImGui::Text("Objects Position :");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Sphere %d", i);

			Sphere& sphere = m_Scene.Spheres[i];

			if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f)
			|| ImGui::DragFloat("Radius", &sphere.Radius, 0.1f)
			|| ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1))
				m_Renderer.ResetFrameIndex();
			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("Texturing");
		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Material %d", i);

			Material& material = m_Scene.Materials[i];

			if (ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo))
			|| ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f)
			|| ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f))
				m_Renderer.ResetFrameIndex();

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("Viewport");
		
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		
		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::End();

		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}