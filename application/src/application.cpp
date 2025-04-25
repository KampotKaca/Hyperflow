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
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	int count;
	hf::Ref<hf::Window> wn;
	int reqCount = 0;
	int32_t lastReq = -1;
	hf::Ref<hf::Shader> shader;
	hf::Ref<hf::VertBuffer> buffer;
	hf::BufferAttrib bufferAttrib;

	constexpr hf::BufferAttribFormat formats[]
	{
		{ .type = hf::VertBufferDataType::F32, .size = 2 },
		{ .type = hf::VertBufferDataType::F32, .size = 3 },
	};

	hf::BufferAttribCreateInfo bufferAttribCreateInfo
	{
		.formatCount = 2,
		.pFormats = formats
	};

	void Application::LoadResources()
	{
		bufferAttrib = hf::vertbuffer::CreateAttrib(bufferAttribCreateInfo);

		hf::VertBufferCreationInfo bufferInfo
		{
			.bufferAttrib = bufferAttrib,
			.vertexCount = 3,
			.vertices = (void*)vertices,
		};

		buffer = hf::vertbuffer::Create(bufferInfo);

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
				.onRenderCallback = OnMainWindowRender
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

		// auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
		// if (cReq != reqCount)
		// {
		// 	std::ostringstream oss;
		// 	oss << "[Hyperflow] " << hf::time::GetFrameRate();
		// 	// oss << "[Hyperflow] " << hf::Time::GetTimePassed();
		// 	hf::window::SetTitle(hf::GetMainWindow(), oss.str());
		// 	reqCount = cReq;
		// }
	}

	void Application::Quit()
	{

	}

	void Application::OnMainWindowRender(const hf::Ref<hf::Renderer>& rn)
	{
		hf::shader::Bind(rn, shader, bufferAttrib);
		hf::renderer::Draw(rn);
	}
}