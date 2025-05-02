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
	hf::UniformBuffer cameraBuffer;
	hf::UniformStorage uniformStorage;
	hf::UniformAllocator uniformAllocator;

	struct Camera
	{
		alignas(16) hf::mat4 model{};
		alignas(16) hf::mat4 view{};
		alignas(16) hf::mat4 proj{};
		alignas(16) hf::mat4 viewProj{};
	};

	Camera camera;

	void Application::OnRendererLoad()
	{
		hf::BufferAttribFormat formats[]
		{
			{ .type = hf::BufferDataType::F32, .size = 2, },
			{ .type = hf::BufferDataType::F32, .size = 3, },
		};

		hf::BufferAttribDefinitionInfo bufferAttribDefinitionInfo
		{
			.bindingId = 0,
			.formatCount = 2,
			.pFormats = formats
		};

		bufferAttrib = hf::bufferattrib::Define(bufferAttribDefinitionInfo);

		hf::UniformBufferDefinitionInfo cameraBufferDefinitionInfo
		{
			.bindingId = 0,
			.usageStageFlags = hf::UniformBufferStage::Vertex | hf::UniformBufferStage::Fragment,
			.arraySize = 1,
			.elementSizeInBytes = sizeof(Camera)
		};

		cameraBuffer = hf::uniformbuffer::Define(cameraBufferDefinitionInfo);

		hf::UniformAllocatorDefinitionInfo uniformAllocatorDefinitionInfo
		{
			.pBuffers = &cameraBuffer,
			.bufferCount = 1,
		};

		uniformAllocator = hf::uniformallocator::Define(uniformAllocatorDefinitionInfo);

		hf::UniformStorageDefinitionInfo uniformStorageDefinitionInfo
		{
			.pBuffers = &cameraBuffer,
			.bufferCount = 1,
		};

		uniformStorage = hf::uniformstorage::Define(uniformStorageDefinitionInfo);
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
			.uniformStorage = uniformStorage,
			.supportedAttribCount = 1,
			.pSupportedAttribs = &bufferAttrib,
			.vertexShaderLoc = "default",
			.fragmentShaderLoc = "default"
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

		if (hf::input::IsDown(hf::Key::T))
		{
			switch (hf::renderer::GetApiType())
			{
			case hf::RenderingApiType::None:
				break;
			case hf::RenderingApiType::Vulkan:
				hf::renderer::ChangeApi(hf::RenderingApiType::Direct3D);
				break;
			case hf::RenderingApiType::Direct3D:
				hf::renderer::ChangeApi(hf::RenderingApiType::Vulkan);
				break;
			}
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
		auto size = hf::renderer::GetSize(rn);
		camera.model = glm::rotate(hf::mat4(1.0f), (float)hf::time::GetTimePassed() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.view = glm::lookAt(hf::vec3(2.0f, 2.0f, 2.0f), hf::vec3(0.0f, 0.0f, 0.0f), hf::vec3(0.0f, 0.0f, 1.0f));
		camera.proj = glm::perspective(glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 10.0f);
		camera.proj[1][1] *= -1;
		camera.viewProj = camera.proj * camera.view;

		hf::uniformstorage::Bind(rn, uniformStorage);

		hf::UniformBufferUpload cameraUpload
		{
			.buffer = cameraBuffer,
			.offsetInBytes = 0,
			.sizeInBytes = sizeof(Camera),
			.data = &camera
		};

		hf::UniformBufferUploadInfo uniformBufferUploads
		{
			.bindingType = hf::UniformBufferBindingType::Graphics,
			.pUploads = &cameraUpload,
			.uploadCount = 1
		};

		hf::uniformbuffer::Upload(rn, uniformBufferUploads);

		hf::shader::Bind(rn, shader, bufferAttrib);

		hf::DrawCallInfo drawCallInfo
		{
			.pVertBuffers = &vertBuffer,
			.bufferCount = 1,
			.indexBuffer = indexBuffer,
			.instanceCount = 1
		};

		hf::renderer::Draw(rn, drawCallInfo);
	}
}
