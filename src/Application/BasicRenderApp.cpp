#include "Application/BasicRenderApp.h"

#include "Core/Mat4.h"
#include "Core/Quat.h"
#include "Core/Vec3.h"
#include "Core/TVec2.h"
#include "Render/Attribute.h"
#include "Render/Draw.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/IndexBuffer.h"
#include "Render/Uniform.h"
#include "Utils/Utils.h"

// ---------------------------------------------------------------------------------------------------------------------

void BasicRenderApp::Initialize()
{
    Application::Initialize();
	
	m_Shader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	m_DisplayTexture = new Texture("Assets/uv.png");

	m_VertexPositions = new Attribute<Vec3>();
	m_VertexNormals = new Attribute<Vec3>();
	m_VertexTexCoords = new Attribute<Vec2>();
	m_IndexBuffer = new IndexBuffer();

	std::vector<Vec3> positions;
	positions.emplace_back(Vec3(-1, -1, 0));
	positions.emplace_back(Vec3(-1, 1, 0));
	positions.emplace_back(Vec3(1, -1, 0));
	positions.emplace_back(Vec3(1, 1, 0));
	m_VertexPositions->Set(positions);

	std::vector<Vec3> normals;
	normals.resize(4, Vec3(0, 0, 1));
	m_VertexNormals->Set(normals);

	std::vector<Vec2> uvs;
	uvs.emplace_back(Vec2(0, 0));
	uvs.emplace_back(Vec2(0, 1));
	uvs.emplace_back(Vec2(1, 0));
	uvs.emplace_back(Vec2(1, 1));
	m_VertexTexCoords->Set(uvs);

	std::vector<unsigned int> indices;
	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);
	indices.emplace_back(2);
	indices.emplace_back(1);
	indices.emplace_back(3);
	m_IndexBuffer->Set(indices);
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void BasicRenderApp::Update(float inDeltaTime)
{
    Application::Update(inDeltaTime);

	m_Rotation += inDeltaTime * 45.0f;
	while (m_Rotation > 360.0f)
	{
		m_Rotation -= 360.0f;
	}
	
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void BasicRenderApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

	Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	Mat4 view = Mat4::CreateLookAt(Vec3(0, 0, -5), Vec3(0, 0, 0), Vec3(0, 1, 0));
	Mat4 model = Quat::CreateFromAxis(Utils::DegToRad(m_Rotation), Vec3(0, 0, 1)).ToMat4();

	m_Shader->Bind();

	m_VertexPositions->BindTo(m_Shader->GetAttribute("position"));
	m_VertexNormals->BindTo(m_Shader->GetAttribute("normal"));
	m_VertexTexCoords->BindTo(m_Shader->GetAttribute("texCoord"));

	Uniform<Mat4>::Set(m_Shader->GetUniform("model"), model);
	Uniform<Mat4>::Set(m_Shader->GetUniform("view"), view);
	Uniform<Mat4>::Set(m_Shader->GetUniform("projection"), projection);

	Uniform<Vec3>::Set(m_Shader->GetUniform("light"), Vec3(0, 0, 1));

	m_DisplayTexture->Set(m_Shader->GetUniform("tex0"), 0);

	DrawLibrary::Draw(*m_IndexBuffer, DrawMode::Triangles);

	m_DisplayTexture->Unset(0);

	m_VertexPositions->UnbindFrom(m_Shader->GetAttribute("position"));
	m_VertexNormals->UnbindFrom(m_Shader->GetAttribute("normal"));
	m_VertexTexCoords->UnbindFrom(m_Shader->GetAttribute("texCoord"));

	m_Shader->Unbind();
	
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void BasicRenderApp::Shutdown()
{
	delete m_Shader;
	delete m_DisplayTexture;
	delete m_VertexPositions;
	delete m_VertexNormals;
	delete m_VertexTexCoords;
	delete m_IndexBuffer;
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------
