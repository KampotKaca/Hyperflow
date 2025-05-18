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

	uint32_t count = 0;
	uint32_t currentTexture = 0;
	uint32_t reqCount = 0;
	hf::Ref<hf::Window> wn{};
	int32_t lastReq = -1;
	hf::Ref<hf::Shader> shader{};
	hf::Ref<hf::Mesh> mesh{};

	hf::Ref<hf::Texture> textures[2]{};
	hf::Ref<hf::TexturePack> texPack{};
	hf::Ref<hf::TexturePackAllocator> texPackAllocator{};

	hf::BufferAttrib bufferAttrib{};
	hf::UniformBuffer cameraBuffer{};
	hf::ShaderSetup shaderSetup{};
	hf::UniformAllocator uniformAllocator{};
	hf::TextureSampler sampler{};
	hf::TextureLayout layout{};
	hf::RenderPass presentPass{};

	struct Camera
	{
		alignas(16) hf::mat4 model{};
		alignas(16) hf::mat4 view{};
		alignas(16) hf::mat4 proj{};
		alignas(16) hf::mat4 viewProj{};
	};

	Camera camera{};

	hf::RenderPass Application::OnPassCreationCallback(const hf::Ref<hf::Renderer>& rn)
	{
		wn = hf::GetMainWindow();

		hf::RenderPassDependencyInfo dependencyInfo
		{
			.src =
			{
				.stageMask = hf::RenderPassStage::ColorAttachmentOutput |
							 hf::RenderPassStage::EarlyFragmentTest,
			},
			.dst =
			{
				.stageMask = hf::RenderPassStage::ColorAttachmentOutput |
							 hf::RenderPassStage::EarlyFragmentTest,
				.accessMask = hf::AccessType::ColorAttachmentWrite |
					          hf::AccessType::DepthStencilAttachmentWrite
			},
			.flags = hf::RenderPassDependencyType::ByRegion
		};
		hf::RenderSubpassPresentationAttachmentInfo presentAttachment{};
		hf::RenderSubpassDepthAttachmentInfo depthAttachment
		{
			.layout = hf::TextureResultLayoutType::DepthStencil,
			.lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
			.lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
			.initialLayout = hf::TextureResultLayoutType::Undefined,
			.finalLayout = hf::TextureResultLayoutType::DepthStencil,
			.usesSharedMemory = false
		};

		hf::RenderSubpassInfo subpassInfo
		{
			.bindingType = hf::RenderBindingType::Graphics,
			.pColorAttachments = nullptr,
			.attachmentCount = 0,
			.msaaCounter = 0,
			.presentationAttachment = &presentAttachment,
			.depthAttachment = &depthAttachment
		};

		hf::RenderPassDefinitionInfo drawPassDefinitionInfo
		{
			.pSubpasses = &subpassInfo,
			.subpassCount = 1,
			.pDependencies = &dependencyInfo,
			.dependencyCount = 1,
		};
		presentPass = hf::renderpass::Define(drawPassDefinitionInfo);
		hf::renderpass::Bind(rn, presentPass);
		return presentPass;
	}

	hf::RenderPass OnSubPassCreationCallback(const hf::Ref<hf::Renderer>& rn)
	{
		hf::renderpass::Bind(rn, presentPass);
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
			.comparison = hf::ComparisonOperation::None,
			.mipMaps =
			{
				.mode = hf::MipMapMode::Linear,
				.minLod = 0.0f,
				.maxLod = 8.0f,
				.lodBias = 0.0f
			}
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
		hf::MeshCreationInfo meshInfo
		{
			.filePath = "viking_room.obj",
			.stats =
			{
				.typeFlags = hf::MeshDataType::Position | hf::MeshDataType::Color | hf::MeshDataType::TexCoord,
				.memoryType = hf::BufferMemoryType::Static,
				.bufferAttrib = bufferAttrib
			}
		};
		mesh = hf::mesh::Create(meshInfo);

		hf::buffer::SubmitAll();

		hf::TextureCreationInfo texInfo
		{
			.filePath = "viking_room.png",
			.desiredChannel = hf::TextureChannel::RGBA,
			.mipLevels = 8,
			.details
			{
				.type = hf::TextureType::Tex2D,
				.format = hf::TextureFormat::R8G8B8A8_Srgb,
				.aspectFlags = hf::TextureAspectFlags::Color,
				.tiling = hf::TextureTiling::Optimal,
				.usage = hf::TextureUsageFlags::Sampled,
				.memoryType = hf::BufferMemoryType::Static,
				.finalLayout = hf::TextureResultLayoutType::ShaderReadOnly
			}
		};

		textures[0] = hf::texture::Create(texInfo);

		hf::TextureCreationInfo secondTexInfo
		{
			.filePath = "greek_head.jpg",
			.desiredChannel = hf::TextureChannel::RGBA,
			.mipLevels = 8,
			.details
			{
				.type = hf::TextureType::Tex2D,
				.format = hf::TextureFormat::R8G8B8A8_Srgb,
				.aspectFlags = hf::TextureAspectFlags::Color,
				.tiling = hf::TextureTiling::Optimal,
				.usage = hf::TextureUsageFlags::Sampled,
				.memoryType = hf::BufferMemoryType::Static,
				.finalLayout = hf::TextureResultLayoutType::ShaderReadOnly
			}
		};
		textures[1] = hf::texture::Create(secondTexInfo);

		hf::TexturePackBindingInfo binding
		{
			.bindingId = 0,
			.sampler = sampler,
			.pTextures = &textures[currentTexture],
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
		hf::texture::SubmitAll();

		hf::TexturePackAllocatorCreationInfo texPackAllocInfo
		{
			.pTexturePacks = &texPack,
			.texturePackCount = 1
		};

		texPackAllocator = hf::texturepackallocator::Create(texPackAllocInfo);

		hf::ShaderCreationInfo shaderInfo
		{
			.renderPass = presentPass,
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
				.vSyncOn = true,
				.rnEventInfo =
				{
					.onPassCreationCallback = OnSubPassCreationCallback,
					.onPreRenderCallback = OnPreRender,
					.onRenderCallback = OnRender
				},
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
		// currentTexture++;
		// currentTexture %= 2;
		// hf::texturepack::SetBindingTextures(texPack, 0, &textures[currentTexture], 1, 0);
	}

	void Application::OnRender(const hf::Ref<hf::Renderer>& rn)
	{
		auto size = hf::renderer::GetSize(rn);
		camera.model = glm::rotate(hf::mat4(1.0f), (float)hf::time::GetTimePassed() * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.view = glm::lookAt(hf::vec3(2.0f, 2.0f, 2.0f), hf::vec3(0.0f, 0.0f, 0.0f), hf::vec3(0.0f, 0.0f, 1.0f));
		camera.proj = glm::perspective(glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 10.0f);
		camera.proj[1][1] *= -1;
		camera.viewProj = camera.proj * camera.view;

		hf::renderpass::Begin(rn, presentPass);

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

		hf::renderer::Draw(rn, mesh);

		hf::renderpass::End(rn);
	}
}
