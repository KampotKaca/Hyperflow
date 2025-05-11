#include "application.h"
#include <hyperflow.h>
#include <sstream>

namespace app
{
	struct Vertex
	{
		hf::vec3 pos{};
		hf::vec3 color{};
		hf::vec2 texCoord{};
	};

	constexpr Vertex vertices[]
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	constexpr uint16_t indices[]
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	int count;
	hf::Ref<hf::Window> wn;
	int reqCount = 0;
	int32_t lastReq = -1;
	hf::Ref<hf::Shader> shader;
	hf::Ref<hf::VertBuffer> vertBuffer;
	hf::Ref<hf::IndexBuffer> indexBuffer;

	hf::Ref<hf::Texture> texture;
	hf::Ref<hf::TextureAllocator> textureAllocator;
	hf::Ref<hf::TexturePack> texPack;
	hf::Ref<hf::TexturePackAllocator> texPackAllocator;

	hf::BufferAttrib bufferAttrib;
	hf::UniformBuffer cameraBuffer;
	hf::ShaderSetup shaderSetup;
	hf::UniformAllocator uniformAllocator;
	hf::TextureSampler sampler;
	hf::TextureLayout layout;
	hf::RenderPass presentPass;

	struct Camera
	{
		alignas(16) hf::mat4 model{};
		alignas(16) hf::mat4 view{};
		alignas(16) hf::mat4 proj{};
		alignas(16) hf::mat4 viewProj{};
	};

	Camera camera;

	hf::RenderPass Application::OnPassCreationCallback()
	{
		hf::RenderPassDependencyInfo dependencyInfo{};

		hf::RenderSubpassAttachmentInfo colorAttachment{};

		hf::RenderSubpassInfo subpassInfo
		{
			.bindingType = hf::RenderBindingType::Graphics,
			.pAttachments = &colorAttachment,
			.attachmentCount = 1,
			.depthAttachment = nullptr
		};

		hf::RenderPassDefinitionInfo drawPassDefinitionInfo
		{
			.pSubpasses = &subpassInfo,
			.subpassCount = 1,
			.pDependencies = &dependencyInfo,
			.dependencyCount = 1,
			.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f },
			.depth = 0.0f,
			.stencil = 0
		};
		presentPass = hf::drawpass::Define(drawPassDefinitionInfo);
		return presentPass;
	}

	void Application::OnRendererLoad()
	{
		hf::BufferAttribFormat formats[]
		{
			{ .type = hf::BufferDataType::F32, .size = 3, },
			{ .type = hf::BufferDataType::F32, .size = 3, },
			{ .type = hf::BufferDataType::F32, .size = 2, },
		};

		hf::BufferAttribDefinitionInfo bufferAttribDefinitionInfo
		{
			.bindingId = 0,
			.formatCount = 3,
			.pFormats = formats
		};

		bufferAttrib = hf::bufferattrib::Define(bufferAttribDefinitionInfo);

		hf::UniformBufferBindingInfo bufferBindingInfo
		{
			.type = hf::UniformBufferType::UniformBuffer,
			.usageFlags = hf::ShaderUsageStage::Default,
			.arraySize = 1,
			.elementSizeInBytes = sizeof(Camera)
		};

		hf::UniformBufferDefinitionInfo cameraBufferDefinitionInfo
		{
			.bindingId = 0,
			.pBindings = &bufferBindingInfo,
			.bindingCount = 1
		};

		cameraBuffer = hf::uniformbuffer::Define(cameraBufferDefinitionInfo);

		hf::UniformAllocatorDefinitionInfo uniformAllocatorDefinitionInfo
		{
			.pBuffers = &cameraBuffer,
			.bufferCount = 1,
		};

		uniformAllocator = hf::uniformallocator::Define(uniformAllocatorDefinitionInfo);

		hf::TextureLayoutBindingInfo textureLayoutBindingInfo
		{
			.bindingId = 0,
			.type = hf::TextureLayoutType::CombinedImageSampler,
			.usageFlags = hf::ShaderUsageStage::Default,
			.arraySize = 1,
		};

		hf::TextureLayoutDefinitionInfo textureLayoutDefinitionInfo
		{
			.pBindings = &textureLayoutBindingInfo,
			.bindingCount = 1
		};

		layout = hf::texturelayout::Define(textureLayoutDefinitionInfo);

		hf::TextureSamplerDefinitionInfo samplerDefinitionInfo
		{
			.filter = hf::TextureFilter::Bilinear,
			.anisotropicFilter = hf::TextureAnisotropicFilter::X16,
			.repeatMode = hf::TextureRepeatMode::Repeat,
			.useNormalizedCoordinates = true,
			.comparison = hf::ComparisonOperation::None
		};
		sampler = hf::texturesampler::Define(samplerDefinitionInfo);

		hf::ShaderSetupDefinitionInfo shaderSetupDefinitionInfo
		{
			.pBuffers = &cameraBuffer,
			.bufferCount = 1,
			.pTextureLayouts = &layout,
			.textureLayoutCount = 1
		};

		shaderSetup = hf::shadersetup::Define(shaderSetupDefinitionInfo);
	}

	void Application::LoadResources()
	{
		hf::VertBufferCreationInfo vertBufferInfo
		{
			.bufferAttrib = bufferAttrib,
			.memoryType = hf::BufferMemoryType::Static,
			.vertexCount = 8,
			.pVertices = (void*)vertices,
		};

		vertBuffer = hf::vertbuffer::Create(vertBufferInfo);

		hf::IndexBufferCreationInfo indexBufferInfo
		{
			.indexFormat = hf::BufferDataType::U16,
			.memoryType = hf::BufferMemoryType::Static,
			.indexCount = 12,
			.pIndices = (void*)indices,
		};

		indexBuffer = hf::indexbuffer::Create(indexBufferInfo);

		hf::TextureCreationInfo texInfo
		{
			.filePath = "greek_head.jpg",
			.type = hf::TextureType::Tex2D,
			.format = hf::TextureFormat::B8G8R8A8_Srgb,
			.desiredChannel = hf::TextureChannel::RGBA,
			.memoryType = hf::BufferMemoryType::Static,
			.mipLevels = 1,
		};

		texture = hf::texture::Create(texInfo);

		hf::TextureAllocatorCreationInfo texAllocInfo
		{
			.pTextures = &texture,
			.textureCount = 1
		};

		textureAllocator = hf::textureallocator::Create(texAllocInfo);

		hf::TexturePackBindingInfo binding
		{
			.bindingId = 0,
			.sampler = sampler,
			.pTextures = &texture,
			.textureCount = 1
		};

		hf::TexturePackCreationInfo texPackInfo
		{
			.bindingType = hf::RenderBindingType::Graphics,
			.setBindingIndex = 1,
			.pBindings = &binding,
			.bindingCount = 1,
			.layout = layout,
		};

		texPack = hf::texturepack::Create(texPackInfo);

		hf::TexturePackAllocatorCreationInfo texPackAllocInfo
		{
			.pTexturePacks = &texPack,
			.texturePackCount = 1
		};

		texPackAllocator = hf::texturepackallocator::Create(texPackAllocInfo);

		hf::ShaderCreationInfo shaderInfo
		{
			.drawPass = presentPass,
			.setup = shaderSetup,
			.texturePack = texPack,
			.supportedAttribCount = 1,
			.pSupportedAttribs = &bufferAttrib,
			.vertexShaderLoc = "default",
			.fragmentShaderLoc = "default",
		};

		shader = hf::shader::Create(shaderInfo);
	}

	void Application::Start()
	{
		hf::time::SetTargetFrameRate(-1);
		count = 0;
		wn = hf::GetMainWindow();
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
			hf::WindowCreationInfo data =
			{
				.title = names[count % 5],
				.flags = hf::WindowFlags::Default,
				.style = hf::WindowStyle::Default,
				.position = { 100, 100 },
				.size = { 200, 200 },
				.onPreRenderCallback = OnPreRender,
				.onRenderCallback = OnRender
			};
			wn = hf::window::Open(data, nullptr);
			count++;
		}
		if (hf::input::IsDown(hf::Key::Space) && wn)
		{
			hf::window::Close(wn);
		}

		if (hf::input::IsDown(hf::Key::K))
		{
			hf::window::SetVSync(wn, !hf::window::IsVSyncOn(wn));
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

		hf::drawpass::Begin(rn, presentPass);

		hf::shadersetup::Bind(rn, shaderSetup);

		hf::UniformBufferUpload cameraUpload
		{
			.buffer = cameraBuffer,
			.offsetInBytes = 0,
			.sizeInBytes = sizeof(Camera),
			.data = &camera
		};

		hf::UniformBufferUploadInfo uniformBufferUploads
		{
			.bindingType = hf::RenderBindingType::Graphics,
			.setBindingIndex = 0,
			.pUploads = &cameraUpload,
			.uploadCount = 1
		};

		hf::uniformbuffer::Upload(rn, uniformBufferUploads);

		hf::shader::Bind(rn, shader, bufferAttrib);
		hf::texturepack::Bind(rn, texPack);

		hf::DrawCallInfo drawCallInfo
		{
			.pVertBuffers = &vertBuffer,
			.bufferCount = 1,
			.indexBuffer = indexBuffer,
			.instanceCount = 1
		};

		hf::renderer::Draw(rn, drawCallInfo);

		hf::drawpass::End(rn);
	}
}
