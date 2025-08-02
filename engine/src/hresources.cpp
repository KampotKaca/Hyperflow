#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    namespace primitives
    {
        Ref<Mesh> GetCubeMesh() { return inter::HF.staticResources.cube; }

        Ref<VertBuffer> GetQuadBuffer() { return inter::HF.staticResources.quadBuffer; }
        Ref<VertBuffer>* GetQuadBufferP() { return &inter::HF.staticResources.quadBuffer; }

        BufferAttrib GetQuadBufferAttrib() { return inter::HF.staticResources.quadAttrib; }
        Buffer GetGlobalUniformBuffer() { return inter::HF.staticResources.globalUniform; }
        Buffer GetMaterialStorageBuffer() { return inter::HF.graphicsResources.materialDataStorageBuffer; }
        void BindGlobalUniformBuffer(const Ref<Renderer>& rn)
        {
            Start_BufferSet(rn, RenderBindingType::Graphics, 0);
            BufferSetAdd_Buffer(rn, inter::HF.staticResources.globalUniform);
            End_BufferSet(rn);
        }

        BufferAttrib GetCubeBufferAttrib() { return inter::HF.staticResources.defaultAttrib; }
        TextureSampler GetCubemapSampler() { return inter::HF.staticResources.cubemapSampler; }

        Ref<Mesh> GetCube() { return inter::HF.staticResources.cube; }
        Ref<Mesh> GetPlane() { return inter::HF.staticResources.plane; }
        Ref<Mesh> GetIcoSphere() { return inter::HF.staticResources.icoSphere; }
        Ref<Mesh> GetUvSphere() { return inter::HF.staticResources.uvSphere; }
        Ref<Material> GetEmptyMaterial() { return inter::HF.staticResources.emptyMaterial; }
    }

    namespace inter::primitives
    {
        static void DefineTextureLayouts();
        static void DefineBufferAttribs();
        static void DefineTextureSamplers();
        static void DefineBuffers();
        static void DefineShaderLayouts();

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
            DefineShaderLayouts();
        }

        void LoadStaticResources_i()
        {
            LoadShaders();
            LoadMeshes();
            LoadCubemaps();

            SubmitAllTextures();

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

                HF.staticResources.quadAttrib = Define(attribInfo);
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

                HF.staticResources.defaultAttrib = Define(attribInfo);
            }
        }

        void DefineTextureLayouts()
        {
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

                HF.staticResources.skyboxLayout = Define(layoutInfo);
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

                HF.staticResources.cubemapSampler = Define(samplerInfo);
            }
        }

        void DefineBuffers()
        {
            {
                constexpr BufferDefinitionInfo uniform
                {
                    .bindingId = 0,
                    .pBindings = &HF.internalResourcesFormat.globalUniformBindingInfo,
                    .bindingCount = 1
                };

                HF.staticResources.globalUniform = Define(uniform);
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

                HF.graphicsResources.materialDataStorageBuffer = Define(info);
            }

            SubmitAllBuffers();
        }

        void DefineShaderLayouts()
        {
            //Axis Lines Shader Setup
            {
                const ShaderLayoutDefinitionInfo info
                {
                    .pushConstant =
                    {
                        .usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment,
                        .sizeInBytes = sizeof(GridLinesInfo),
                    },
                    .pBuffers = &HF.staticResources.globalUniform,
                    .bufferCount = 1,
                    .pTextureLayouts = nullptr,
                    .textureLayoutCount = 0
                };

                HF.staticResources.axisLinesShaderLayout = Define(info);
            }

            //Skybox Shader Setup
            {
                const ShaderLayoutDefinitionInfo info
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

                HF.staticResources.skyboxShaderLayout = Define(info);
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

                HF.staticResources.defaultSkyboxCubemap = Create(info);
                HF.staticResources.boundCubemap = HF.staticResources.defaultSkyboxCubemap;
            }
        }

        void LoadTexturePacks()
        {
            //Skybox texturepack
            {
                TexturePackBindingInfo<Cubemap>::TextureInfo ti
                {
                    .texture = HF.staticResources.defaultSkyboxCubemap,
                    .index = 0
                };

                TexturePackBindingInfo<Cubemap> binding
                {
                    .sampler = HF.staticResources.cubemapSampler,
                    .textures = &ti,
                    .arraySize = 1,
                    .bindingIndex = 0
                };

                const TexturePackCreationInfo info
                {
                    .pCubemapBindings = &binding,
                    .cubemapBindingCount = 1,
                    .layout = HF.staticResources.skyboxLayout,
                };
                HF.staticResources.skyboxTexturePack = Create(info);
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

                HF.staticResources.quadBuffer = Create(bufferInfo);
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
                        .bufferAttrib = HF.staticResources.defaultAttrib,
                    }
                };
                HF.staticResources.cube = Create(info);
            }

            //Plane mesh
            {
                const MeshCreationInfo info
                {
                    .filePath = "__plane.obj",
                    .stats =
                    {
                        .typeFlags = MeshDataType::Position | MeshDataType::Normal | MeshDataType::TexCoord,
                        .memoryType = BufferMemoryType::Static,
                        .bufferAttrib = HF.staticResources.defaultAttrib,
                    }
                };
                HF.staticResources.plane = Create(info);
            }

            //Ico Sphere mesh
            {
                const MeshCreationInfo info
                {
                    .filePath = "__ico_sphere.obj",
                    .stats =
                    {
                        .typeFlags = MeshDataType::Position | MeshDataType::Normal | MeshDataType::TexCoord,
                        .memoryType = BufferMemoryType::Static,
                        .bufferAttrib = HF.staticResources.defaultAttrib,
                    }
                };
                HF.staticResources.icoSphere = Create(info);
            }

            //Uv Sphere mesh
            {
                const MeshCreationInfo info
                {
                    .filePath = "__uv_sphere.obj",
                    .stats =
                    {
                        .typeFlags = MeshDataType::Position | MeshDataType::Normal | MeshDataType::TexCoord,
                        .memoryType = BufferMemoryType::Static,
                        .bufferAttrib = HF.staticResources.defaultAttrib,
                    }
                };
                HF.staticResources.uvSphere = Create(info);
            }
        }

        void LoadShaders()
        {
            //Shader Library!
            {
                std::array vertexInputModules
                {
                    ShaderLibraryModule<ShaderLibraryVertexInputModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.quadVertexInput,
                        .module =
                        {
                            .pAttributes = { HF.staticResources.quadAttrib },
                            .attributeCount = 1,
                        },
                    },
                    ShaderLibraryModule<ShaderLibraryVertexInputModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.defaultVertexInput,
                        .module =
                        {
                            .pAttributes = { HF.staticResources.defaultAttrib },
                            .attributeCount = 1
                        },
                    },
                };

                std::array preRasterModules
                {
                    ShaderLibraryModule<ShaderLibraryPreRasterModuleInfo> //Axis Lines Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesPreRaster,
                        .module =
                        {
                            .vertexShaderPath = { .path = "__axislines" },
                            .options = ShaderRasterizerOptions
                            {
                                .cullMode = ShaderCullMode::None,
                                .biasOptions = ShaderDepthBiasOptions
                                {
                                    .constantFactor = 1.25f,
                                    .clamp = 0.0f,
                                    .slopeFactor = 1.75f
                                }
                            },
                            .layout = HF.staticResources.axisLinesShaderLayout
                        }
                    },
                    ShaderLibraryModule<ShaderLibraryPreRasterModuleInfo> //Skybox Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxPreRaster,
                        .module =
                        {
                            .vertexShaderPath = { .path = "__skybox" },
                            .options = { .cullMode = ShaderCullMode::Front, },
                            .layout = HF.staticResources.skyboxShaderLayout
                        }
                    },
                };

                std::array fragmentModules
                {
                    ShaderLibraryModule<ShaderLibraryFragmentModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragment,
                        .module =
                        {
                            .fragmentShaderPath = { .path = "__axislines" },
                            .depthStencilOptions =
                            {
                                .enableDepth = true,
                                .writeDepth = true,
                                .comparisonFunc = DepthComparisonFunction::LessOrEqual,
                                .enableDepthBounds = false,
                                .enableStencil = false,
                            },
                            .layout = HF.staticResources.axisLinesShaderLayout,
                        }
                    },
                    ShaderLibraryModule<ShaderLibraryFragmentModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragment,
                        .module =
                        {
                            .fragmentShaderPath = { .path = "__skybox" },
                            .depthStencilOptions =
                            {
                                .enableDepth = true,
                                .writeDepth = false,
                                .comparisonFunc = DepthComparisonFunction::Less,
                                .enableDepthBounds = false,
                                .enableStencil = false,
                            },
                            .layout = HF.staticResources.skyboxShaderLayout
                        }
                    },
                };

                std::array fragmentOutputModules
                {
                    ShaderLibraryModule<ShaderLibraryFragmentOutputModuleInfo> //Axis lines
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragmentOutput,
                        .module
                        {
                            .colorAttachmentsSettings =
                            {
                                ColorAttachmentSettings
                                {
                                    .colorWriteMask = ColorMaskingFlags::All,
                                    .blending = ShaderBlendingOptions{}
                                }
                            },
                            .colorAttachmentCount = 1
                        }
                    },
                    ShaderLibraryModule<ShaderLibraryFragmentOutputModuleInfo> //Skybox
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragmentOutput,
                    },
                };

                const ShaderLibraryCreationInfo info
                {
                    .uniqueLibraryName = "__engine_shader_lib_cache",
                    .outputFormats = HF.internalResourcesFormat.drawOutputFormats,
                    .pVertexInputModules = vertexInputModules.data(),
                    .vertexInputModuleCount = vertexInputModules.size(),
                    .pPreRasterModules = preRasterModules.data(),
                    .preRasterModuleCount = preRasterModules.size(),
                    .pFragmentModules = fragmentModules.data(),
                    .fragmentModuleCount = fragmentModules.size(),
                    .pFragmentOutputModules = fragmentOutputModules.data(),
                    .fragmentOutputModuleCount = fragmentOutputModules.size()
                };
                HF.staticResources.engineShadersLib = Create(info);
            }

            //Axis Lines Shader
            {
                const ShaderCreationInfo shaderInfo
                {
                    .layout = HF.staticResources.axisLinesShaderLayout,
                    .library = HF.staticResources.engineShadersLib,
                    .modules =
                    {
                        .vertexInputModuleId = HF.staticResources.engineShadersLibModules.quadVertexInput,
                        .preRasterModuleId = HF.staticResources.engineShadersLibModules.axisLinesPreRaster,
                        .fragmentModuleId = HF.staticResources.engineShadersLibModules.axisLinesFragment,
                        .fragmentOutputModuleId = HF.staticResources.engineShadersLibModules.axisLinesFragmentOutput
                    }
                };
                HF.staticResources.axisLinesShader = Create(shaderInfo);
            }

            //Skybox Shader
            {
                const ShaderCreationInfo shaderInfo
                {
                    .layout = HF.staticResources.skyboxShaderLayout,
                    .library = HF.staticResources.engineShadersLib,
                    .modules =
                    {
                        .vertexInputModuleId = HF.staticResources.engineShadersLibModules.defaultVertexInput,
                        .preRasterModuleId = HF.staticResources.engineShadersLibModules.skyboxPreRaster,
                        .fragmentModuleId = HF.staticResources.engineShadersLibModules.skyboxFragment,
                        .fragmentOutputModuleId = HF.staticResources.engineShadersLibModules.skyboxFragmentOutput
                    }
                };
                HF.staticResources.skyboxShader = Create(shaderInfo);
            }
        }

        void LoadMaterials()
        {
            const MaterialCreationInfo materialInfo
            {
                .sizeInBytes = 0
            };
            HF.staticResources.emptyMaterial = Create(materialInfo);
        }
    }
}