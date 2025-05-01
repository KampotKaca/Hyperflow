#include "application.h"
#include <hyperflow.h>
#include <sstream>

namespace app
{
	struct Vertex
	{
		hf::vec2 pos{};
		hf::vec3 color{};
	};

	constexpr Vertex vertices[]
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	constexpr uint16_t indices[]
	{
		0, 1, 2, 2, 3, 0
	};

	int count;
	hf::Ref<hf::Window> wn;
	int reqCount = 0;
	int32_t lastReq = -1;
	hf::Ref<hf::Shader> shader;
	hf::Ref<hf::VertBuffer> vertBuffer;
	hf::Ref<hf::IndexBuffer> indexBuffer;
	hf::BufferAttrib bufferAttrib;

	struct Camera
	{
		hf::mat4 model;
		hf::mat4 view;
		hf::mat4 proj;
		hf::mat4 viewProj;
	};

	void Application::RendererPreload()
	{
		hf::BufferAttribFormat formats[]
		{
			{ .type = hf::BufferDataType::F32, .size = 2, },
			{ .type = hf::BufferDataType::F32, .size = 3, },
		};

		hf::BufferAttribDefinitionInfo bufferAttribCreateInfo
		{
			.bindingId = 0,
			.formatCount = 2,
			.pFormats = formats
		};

		bufferAttrib = hf::bufferattrib::Define(bufferAttribCreateInfo);
	}

	void Application::LoadResources()
	{
		hf::VertBufferCreationInfo vertBufferInfo
		{
			.bufferAttrib = bufferAttrib,
			.memoryType = hf::BufferMemoryType::Static,
			.vertexCount = 4,
			.pVertices = (void*)vertices,
		};

		vertBuffer = hf::vertbuffer::Create(vertBufferInfo);

		hf::IndexBufferCreationInfo indexBufferInfo
		{
			.indexFormat = hf::BufferDataType::U16,
			.memoryType = hf::BufferMemoryType::Static,
			.indexCount = 6,
			.pIndices = (void*)indices,
		};

		indexBuffer = hf::indexbuffer::Create(indexBufferInfo);

		hf::ShaderCreationInfo shaderInfo
		{
			.supportedAttribCount = 1,
			.pSupportedAttribs = &bufferAttrib,
			.vertexShaderLoc = "shaders/vulkan/default.vert.spv",
			.fragmentShaderLoc = "shaders/vulkan/default.frag.spv"
		};

		shader = hf::shader::Create(shaderInfo);
	}

	void Application::Start()
	{
		count = 0;
	}

	void Application::Update()
	{
		const std::string names[5] =
		{
			"N1",
			"N2",
			"N3",
			"N4",
			"N5",
		};

		if (hf::input::IsDown(hf::Key::A))
		{
			hf::WindowData data =
			{
				.title = names[count % 5],
				.flags = hf::WindowFlags::Default,
				.style = hf::WindowStyle::Default,
				.position = { 100, 100 },
				.size = { 200, 200 },
				.onRenderCallback = OnRender
			};
			wn = hf::window::Open(data, nullptr);
			count++;
		}
		if (hf::input::IsDown(hf::Key::Space) && wn)
		{
			hf::window::Close(wn);
		}

		// auto delta = hf::input::GetScrollDelta();
		// if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
		//
		// auto mDelta = hf::input::GetPointerDelta();
		// if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

		auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
		if (cReq != reqCount)
		{
			// std::ostringstream oss;
			// oss << "[Hyperflow] " << hf::time::GetFrameRate();
			// oss << "[Hyperflow] " << hf::Time::GetTimePassed();
			std::string str = std::string("[Hyperflow] ") + std::to_string(hf::time::GetFrameRate());
			hf::window::SetTitle(hf::GetMainWindow(), str);
			reqCount = cReq;
		}
	}

	void Application::Quit()
	{

	}

	void Application::OnPreRender(const hf::Ref<hf::Renderer>& rn)
	{

	}

	void Application::OnRender(const hf::Ref<hf::Renderer>& rn)
	{
		hf::shader::Bind(rn, shader, bufferAttrib);
		hf::DrawCallInfo drawCallInfo
		{
			.renderer = rn,
			.pVertBuffers = &vertBuffer,
			.bufferCount = 1,
			.indexBuffer = indexBuffer,
			.instanceCount = 1
		};

		hf::renderer::Draw(drawCallInfo);
	}
}