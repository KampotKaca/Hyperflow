#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    namespace primitives
    {
        BufferAttrib GetQuadBufferAttrib() { return inter::HF.staticResources.quadAttrib; }
        TextureLayout GetEmptyTextureLayout() { return inter::HF.staticResources.emptyLayout; }
        Buffer GetGlobalUniformBuffer() { return inter::HF.staticResources.globalUniform; }
        Buffer GetMaterialStorageBuffer() { return inter::HF.graphicsResources.materialDataStorageBuffer; }
        void BindGlobalUniformBuffer(const Ref<Renderer>& rn)
        {
            rn->Start_BufferSet(RenderBindingType::Graphics, 0);
            rn->BufferSetAdd_Buffer(inter::HF.staticResources.globalUniform);
            rn->End_BufferSet();
        }

        BufferAttrib GetCubeBufferAttrib() { return inter::HF.staticResources.cubeAttrib; }
        TextureSampler GetCubemapSampler() { return inter::HF.staticResources.cubemapSampler; }

        Ref<Mesh> GetCube() { return inter::HF.staticResources.cube; }
        Ref<Material> GetEmptyMaterial() { return inter::HF.staticResources.emptyMaterial; }
    }

    namespace inter::primitives
    {
        static void DefineTextureLayouts();
        static void DefineBufferAttribs();
        static void DefineTextureSamplers();
        static void DefineBuffers();
        static void DefineShaderSetups();

        static void LoadCubemaps();
        static void LoadTexturePacks();
        static void LoadMeshes();
        static void LoadShaders();
        static void LoadMaterials();

        void DefineStaticResources_i()
        {
            DefineTextureLayouts();
            DefineBufferAttribs();
            DefineTextureSamplers();
            DefineBuffers();
            DefineShaderSetups();
        }

        void LoadStaticResources_i()
        {
            LoadShaders();
            LoadMeshes();
            LoadCubemaps();
            LoadTexturePacks();
            LoadMaterials();
        }

        void DefineBufferAttribs()
        {
            //Quat Attribute
            {
                std::array<BufferAttribFormat, 1> formats{};
                formats[0] = { .type = BufferDataType::F32, .size = 2 }; //Position

                const BufferAttribDefinitionInfo attribInfo
                {
                    .bindingId = 0,
                    .formatCount = formats.size(),
                    .pFormats = formats.data()
                };

                HF.staticResources.quadAttrib = DefineBufferAttrib(attribInfo);
            }

            //Cube Attrib
            {
                std::array<BufferAttribFormat, 3> formats{};
                formats[0] = { .type = BufferDataType::F32, .size = 3 }; //Position
                formats[1] = { .type = BufferDataType::F32, .size = 3 }; //Normal
                formats[2] = { .type = BufferDataType::F32, .size = 2 }; //TexCoord

                BufferAttribDefinitionInfo attribInfo
                {
                    .bindingId = 0,
                    .formatCount = formats.size(),
                    .pFormats = formats.data()
                };

                HF.staticResources.cubeAttrib = DefineBufferAttrib(attribInfo);
            }
        }

        void DefineTextureLayouts()
        {
            //Empty Texture Layout
            {
                constexpr TextureLayoutDefinitionInfo layoutInfo
                {
                    .pBindings = nullptr,
                    .bindingCount = 0
                };

                HF.staticResources.emptyLayout = DefineTextureLayout(layoutInfo);
            }

            //Skybox Texture Layout
            {
                TextureLayoutBindingInfo cubemapBinding
                {
                    .bindingId = 0,
                    .usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment,
                    .arraySize = 1
                };

                const TextureLayoutDefinitionInfo layoutInfo
                {
                    .pBindings = &cubemapBinding,
                    .bindingCount = 1
                };

                HF.staticResources.skyboxLayout = DefineTextureLayout(layoutInfo);
            }
        }

        void DefineTextureSamplers()
        {
            //Cubemap Sampler
            {
                constexpr TextureSamplerDefinitionInfo samplerInfo
                {
                    .filter = TextureFilter::Bilinear,
                    .anisotropicFilter = TextureAnisotropicFilter::X16,
                    .repeatMode = TextureRepeatMode::ClampToEdge,
                    .useNormalizedCoordinates = true,
                    .comparison = ComparisonOperation::Never,
                };

                HF.staticResources.cubemapSampler = DefineTextureSampler(samplerInfo);
            }
        }

        void DefineBuffers()
        {
            {
                constexpr BufferDefinitionInfo uniform
                {
                    .bindingId = 0,
                    .pBindings = &HF.globalUniformBindingInfo,
                    .bindingCount = 1
                };

                HF.staticResources.globalUniform = DefineUniformBuffer(uniform);
            }

            {
                BufferBindingInfo bindingInfo
                {
                    .usageFlags = ShaderUsageStage::All,
                    .arraySize = 1,
                    .elementSizeInBytes = 64 * 64 * RENDERING_MAX_MATERIAL_OCTREE_COUNT * RENDERING_MAX_MATERIAL_MEMORY_BADGET
                };
                const StorageBufferDefinitionInfo info
                {
                    .bufferInfo =
                    {
                        .bindingId = 0,
                        .pBindings = &bindingInfo,
                        .bindingCount = 1
                    },
                    .memoryType = BufferMemoryType::WriteOnly,
                    .data = nullptr
                };

                HF.graphicsResources.materialDataStorageBuffer = DefineStorageBuffer(info);
            }

            //allocator
            {
                std::array buffers = { HF.staticResources.globalUniform, HF.graphicsResources.materialDataStorageBuffer };

                const BufferAllocatorDefinitionInfo info
                {
                    .pBuffers = buffers.data(),
                    .bufferCount = buffers.size(),
                };

                HF.staticResources.bufferAllocator = DefineBufferAllocator(info);
            }
        }

        void DefineShaderSetups()
        {
            //Axis Lines Shader Setup
            {
                const ShaderSetupDefinitionInfo info
                {
                    .pushConstant =
                    {
                        .usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment,
                        .sizeInBytes = sizeof(GridLinesInfo),
                    },
                    .pBuffers = &HF.staticResources.globalUniform,
                    .bufferCount = 1,
                    .pTextureLayouts = &HF.staticResources.emptyLayout,
                    .textureLayoutCount = 1
                };

                HF.staticResources.axisLinesShaderSetup = DefineShaderSetup(info);
            }

            //Skybox Shader Setup
            {
                const ShaderSetupDefinitionInfo info
                {
                    .pushConstant =
                    {
                        .usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment,
                        .sizeInBytes = 0,
                    },
                    .pBuffers = &HF.staticResources.globalUniform,
                    .bufferCount = 1,
                    .pTextureLayouts = &HF.staticResources.skyboxLayout,
                    .textureLayoutCount = 1
                };

                HF.staticResources.skyboxShaderSetup = DefineShaderSetup(info);
            }
        }

        void LoadCubemaps()
        {
            //Skybox cubemap
            {
                constexpr CubemapCreationInfo info
                {
                    .folderPath = "__toony",
                    .desiredChannel = TextureChannel::RGBA,
                    .mipLevels = 1,
                    .texturePaths =
                    {
                        .left =  "left.png",
                        .right = "right.png",
                        .down =  "down.png",
                        .up =    "up.png",
                        .back =  "back.png",
                        .front = "front.png"
                    },
                    .details =
                    {
                        .format = TextureFormat::R8G8B8A8_Srgb,
                        .aspectFlags = TextureAspectFlags::Color,
                        .tiling = TextureTiling::Optimal,
                        .usageFlags = TextureUsageFlags::Sampled,
                        .memoryType = BufferMemoryType::Static,
                        .finalLayout = TextureResultLayoutType::ShaderReadOnly,
                    }
                };

                HF.staticResources.defaultSkyboxCubemap = Cubemap::Create(info);
                HF.staticResources.boundCubemap = HF.staticResources.defaultSkyboxCubemap;
            }
        }

        void LoadTexturePacks()
        {
            //Skybox texturepack
            {
                TexturePackCubemapBindingInfo binding
                {
                    .sampler = HF.staticResources.cubemapSampler,
                    .pCubemaps = &HF.staticResources.defaultSkyboxCubemap,
                    .arraySize = 1
                };
                const TexturePackCreationInfo info
                {
                    .bindingType = RenderBindingType::Graphics,
                    .bindingId = 0,
                    .pCubemapeBindings = &binding,
                    .cubemapBindingCount = 1,
                    .layout = HF.staticResources.skyboxLayout,
                };
                HF.staticResources.skyboxTexturePack = TexturePack::Create(info);
            }

            TexturePack::SubmitAll();

            {
                std::array texPacks = { HF.staticResources.skyboxTexturePack };
                TexturePackAllocatorCreationInfo info
                {
                    .pTexturePacks = texPacks.data(),
                    .texturePackCount = texPacks.size()
                };
                HF.staticResources.texPackAllocator = TexturePackAllocator::Create(info);
            }
        }

        void LoadMeshes()
        {
            //Quad Vertices
            {
                vec2 vertices[6]
                {
                    { -1, -1 }, { 1, 1 }, { -1, 1 },
                    { -1, -1 }, { 1, -1 }, { 1, 1 },
                };

                const VertBufferCreationInfo bufferInfo
                {
                    .bufferAttrib = HF.staticResources.quadAttrib,
                    .memoryType = BufferMemoryType::Static,
                    .usageFlags = BufferUsageType::All,
                    .vertexCount = 6,
                    .pVertices = vertices
                };

                HF.staticResources.quadBuffer = VertBuffer::Create(bufferInfo);
            }

            //Cube Mesh
            {
                const MeshCreationInfo info
                {
                    .filePath = "__cube.obj",
                    .stats =
                    {
                        .typeFlags = MeshDataType::Position | MeshDataType::Normal | MeshDataType::TexCoord,
                        .memoryType = BufferMemoryType::Static,
                        .bufferAttrib = HF.staticResources.cubeAttrib,
                    }
                };
                HF.staticResources.cube = Mesh::Create(info);
            }
        }

        void LoadShaders()
        {
            //Axis Lines Shader
            {
                const ShaderCreationInfo shaderInfo
                {
                    .renderPass = HF.mainWindow->renderer->mainPass,
                    .setup = HF.staticResources.axisLinesShaderSetup,
                    .supportedAttribCount = 1,
                    .pSupportedAttribs = &HF.staticResources.quadAttrib,
                    .vertexShaderLoc = "__axislines",
                    .fragmentShaderLoc = "__axislines",
                    .rasterizerOptions =
                    {
                        .cullMode = ShaderCullMode::None,
                    },
                    .alphaTestOptions =
                    {
                        .blendMode = ShaderBlendMode::Alpha,
                        .blendOp = ShaderBlendOp::XOr
                    },
                    .depthStencilOptions =
                    {
                        .enableDepth = true,
                        .writeDepth = true,
                        .comparisonFunc = DepthComparisonFunction::LessOrEqual,
                        .enableDepthBounds = false,
                        .enableStencil = false,
                    }
                };
                HF.staticResources.axisLinesShader = Shader::Create(shaderInfo);
            }

            //Skybox Shader
            {
                const ShaderCreationInfo shaderInfo
                {
                    .renderPass = HF.mainWindow->renderer->mainPass,
                    .setup = HF.staticResources.skyboxShaderSetup,
                    .supportedAttribCount = 1,
                    .pSupportedAttribs = &HF.staticResources.cubeAttrib,
                    .vertexShaderLoc = "__skybox",
                    .fragmentShaderLoc = "__skybox",
                    .rasterizerOptions =
                    {
                        .cullMode = ShaderCullMode::Front,
                    },
                    .alphaTestOptions =
                    {
                        .blendMode = ShaderBlendMode::Alpha,
                        .blendOp = ShaderBlendOp::XOr
                    },
                    .depthStencilOptions =
                    {
                        .enableDepth = true,
                        .writeDepth = false,
                        .comparisonFunc = DepthComparisonFunction::Less,
                        .enableDepthBounds = false,
                        .enableStencil = false,
                    }
                };
                HF.staticResources.skyboxShader = Shader::Create(shaderInfo);
            }
        }

        void LoadMaterials()
        {
            const MaterialCreationInfo materialInfo
            {
                .sizeInBytes = 0
            };
            HF.staticResources.emptyMaterial = Material::Create(materialInfo);
        }
    }
}