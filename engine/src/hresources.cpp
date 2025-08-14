#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    namespace primitives
    {
        Ref<VertexBuffer> GetQuadBuffer()   { return inter::HF.staticResources.quadBuffer; }
        Ref<VertexBuffer>* GetQuadBufferP() { return &inter::HF.staticResources.quadBuffer; }

        Buffer GetGlobalUniformBuffer()     { return inter::HF.staticResources.globalUniform; }
        Buffer GetMaterialStorageBuffer()   { return inter::HF.graphicsResources.materialDataStorageBuffer; }
        void BindGlobalUniformBuffer(const Ref<Renderer>& rn)
        {
            Start_BufferSet(rn, RenderBindingType::Graphics, 0);
            BufferSetAdd_Buffer(rn, inter::HF.staticResources.globalUniform);
            End_BufferSet(rn);
        }

        Ref<Mesh> GetMesh(PrimitiveMeshType type) { return inter::HF.staticResources.primitiveMeshes[(uint32_t)type]; }
        Ref<Texture> GetTexture(PrimitiveTextureType type) { return inter::HF.staticResources.primitiveTextures[(uint32_t)type]; }

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
        static void LoadTextures();
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
            LoadTextures();
            LoadCubemaps();

            SubmitAllTextures();

            LoadTexturePacks();
            LoadMaterials();
        }

        void DefineBufferAttribs()
        {
            HF.staticResources.vertexAttributes.quad = DefineVertexAttributeAsset("__quad");
            HF.staticResources.vertexAttributes.pos_nor_tex = DefineVertexAttributeAsset("__pos_nor_tex");
        }

        void DefineTextureLayouts()
        {
            HF.staticResources.skyboxLayout = DefineTextureLayoutAsset("__skybox");
        }

        void DefineTextureSamplers()
        {
            HF.staticResources.cubemapSampler = DefineTextureSamplerAsset("__cubemap");
        }

        void DefineBuffers()
        {
            {
                BufferDefinitionInfo uniform{};
                uniform.bindingId = 0;
                uniform.pBindings = &HF.internalResourcesFormat.globalUniformBindingInfo;
                uniform.bindingCount = 1;

                HF.staticResources.globalUniform = Define(uniform);
            }

            {
                BufferBindingInfo bindingInfo{};
                bindingInfo.usageFlags = ShaderUsageStageFlags::All;
                bindingInfo.arraySize = 1;
                bindingInfo.elementSizeInBytes = 64 * 64 * RENDERING_MAX_MATERIAL_OCTREE_COUNT * RENDERING_MAX_MATERIAL_MEMORY_BADGET;

                BufferDefinitionInfo defInfo{};
                defInfo.bindingId = 0;
                defInfo.pBindings = &bindingInfo;
                defInfo.bindingCount = 1;

                StorageBufferDefinitionInfo info{};
                info.bufferInfo = defInfo;
                info.memoryType = BufferMemoryType::WriteOnly;

                HF.graphicsResources.materialDataStorageBuffer = Define(info);
            }

            SubmitAllBuffers();
        }

        void DefineShaderLayouts()
        {
            //Axis Lines Shader Setup
            {
                PushConstantInfo pushConstantInfo{};
                pushConstantInfo.usageFlags = ShaderUsageStageFlags::Vertex |
                                              ShaderUsageStageFlags::Fragment;
                pushConstantInfo.sizeInBytes = sizeof(GridLinesInfo);

                ShaderLayoutDefinitionInfo info{};
                info.pushConstant = pushConstantInfo;
                info.pBuffers = &HF.staticResources.globalUniform;
                info.bufferCount = 1;

                HF.staticResources.shaderLayouts.axisLines = Define(info);
            }

            //Skybox Shader Setup
            {
                PushConstantInfo pushConstantInfo{};
                pushConstantInfo.usageFlags = ShaderUsageStageFlags::Vertex |
                                              ShaderUsageStageFlags::Fragment;
                pushConstantInfo.sizeInBytes = 0;

                ShaderLayoutDefinitionInfo info{};
                info.pushConstant = pushConstantInfo;
                info.pBuffers = &HF.staticResources.globalUniform;
                info.bufferCount = 1;
                info.pTextureLayouts = &HF.staticResources.skyboxLayout;
                info.textureLayoutCount = 1;

                HF.staticResources.shaderLayouts.skybox = Define(info);
            }
        }

        void LoadCubemaps()
        {
            HF.staticResources.skyboxResources.defaultCubemap = CreateCubemapAsset("__toony");
            HF.staticResources.skyboxResources.boundCubemap = HF.staticResources.skyboxResources.defaultCubemap;
        }

        void LoadTexturePacks()
        {
            //Skybox texturepack
            {
                TexturePackBindingInfo<Cubemap>::TextureInfo ti{};
                ti.texture = HF.staticResources.skyboxResources.defaultCubemap;
                ti.index = 0;

                TexturePackBindingInfo<Cubemap> binding{};
                binding.sampler = HF.staticResources.cubemapSampler;
                binding.textures = &ti;
                binding.arraySize = 1;
                binding.bindingIndex = 0;

                TexturePackCreationInfo info{};
                info.pCubemapBindings = &binding;
                info.cubemapBindingCount = 1;
                info.layout = HF.staticResources.skyboxLayout;

                HF.staticResources.skyboxResources.texturePack = Create(info);
            }
        }

        void LoadMeshes()
        {
            //Quad Vertices
            {
                vec2 vertices[6]
                {
                    vec2{ -1, -1 }, vec2{ 1, 1 },  vec2{ -1, 1 },
                    vec2{ -1, -1 }, vec2{ 1, -1 }, vec2{ 1, 1 },
                };

                VertexBufferCreationInfo bufferInfo{};
                bufferInfo.vertexCount = 6;
                bufferInfo.vertexSize = GetVertexSize(HF.staticResources.vertexAttributes.quad);
                bufferInfo.memoryType = BufferMemoryType::Static;
                bufferInfo.usageFlags = BufferUsageTypeFlags::All;
                bufferInfo.pVertices = vertices;

                HF.staticResources.quadBuffer = Create(bufferInfo);
            }

            //Instance Buffer
            {
                VertexBufferCreationInfo bufferInfo{};
                bufferInfo.vertexCount = MAX_INSTANCE_BUFFER_SIZE_BYTES / MAX_PER_INSTANCE_BUFFER_SIZE;
                bufferInfo.vertexSize = MAX_PER_INSTANCE_BUFFER_SIZE;
                bufferInfo.memoryType = BufferMemoryType::PerFrameWriteOnly;
                bufferInfo.usageFlags = BufferUsageTypeFlags::All;
                bufferInfo.pVertices = nullptr;

                HF.staticResources.instanceBuffer = Create(bufferInfo);
            }

            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::Cube]      = CreateMeshAsset("__cube.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::IcoSphere] = CreateMeshAsset("__ico_sphere.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::Plane]     = CreateMeshAsset("__plane.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::UVSphere]  = CreateMeshAsset("__uv_sphere.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::Cone]      = CreateMeshAsset("__cone.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::Cylinder]  = CreateMeshAsset("__cylinder.obj");
            HF.staticResources.primitiveMeshes[(uint32_t)PrimitiveMeshType::Torus]     = CreateMeshAsset("__torus.obj");
        }

        void LoadTextures()
        {
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Default]           = CreateTextureAsset("__default.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Circle]            = CreateTextureAsset("__circle.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Triangle]          = CreateTextureAsset("__triangle.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Capsule]           = CreateTextureAsset("__capsule.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::HexagonFlatTop]    = CreateTextureAsset("__hexagon_flat_top.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::HexagonPointedTop] = CreateTextureAsset("__hexagon_pointed_top.png");
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::IsometricDiamond]  = CreateTextureAsset("__isometric_diamond.png");
        }

        void LoadShaders()
        {
            //Shader Library!
            {
                //--------- Vertex Input ------------------------------------------------

                std::array vertexInputModules
                {
                    ShaderLibraryModule<ShaderLibraryVertexInputModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.quadVertexInput,
                    },
                    ShaderLibraryModule<ShaderLibraryVertexInputModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.defaultVertexInput,
                    },
                };

                utils::ReadVertexInputModule("__quad", vertexInputModules[0].module);
                utils::ReadVertexInputModule("__default", vertexInputModules[1].module);

                //--------- Pre Raster ------------------------------------------------

                std::array preRasterModules
                {
                    ShaderLibraryModule<ShaderLibraryPreRasterModuleInfo> //Axis Lines Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesPreRaster,
                    },
                    ShaderLibraryModule<ShaderLibraryPreRasterModuleInfo> //Skybox Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxPreRaster,
                    },
                };

                utils::ReadPreRasterModule("__axislines", HF.staticResources.shaderLayouts.axisLines, preRasterModules[0].module);
                utils::ReadPreRasterModule("__skybox", HF.staticResources.shaderLayouts.skybox, preRasterModules[1].module);

                //--------- Fragment ------------------------------------------------

                std::array fragmentModules
                {
                    ShaderLibraryModule<ShaderLibraryFragmentModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragment,
                    },
                    ShaderLibraryModule<ShaderLibraryFragmentModuleInfo>
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragment,
                    },
                };

                utils::ReadFragmentModule("__axislines", HF.staticResources.shaderLayouts.axisLines, fragmentModules[0].module);
                utils::ReadFragmentModule("__skybox", HF.staticResources.shaderLayouts.skybox, fragmentModules[1].module);

                //--------- Fragment Output ------------------------------------------------

                std::array fragmentOutputModules
                {
                    ShaderLibraryModule<ShaderLibraryFragmentOutputModuleInfo> //Axis lines
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragmentOutput,
                    },
                    ShaderLibraryModule<ShaderLibraryFragmentOutputModuleInfo> //Skybox
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragmentOutput,
                    },
                };

                utils::ReadFragmentOutputModule("__axislines", fragmentOutputModules[0].module);
                utils::ReadFragmentOutputModule("__skybox", fragmentOutputModules[1].module);

                ShaderLibraryCreationInfo info{};
                info.uniqueLibraryName         = "__engine_shader_lib_cache";
                info.outputFormats             = HF.internalResourcesFormat.drawOutputFormats;
                info.pVertexInputModules       = vertexInputModules.data();
                info.vertexInputModuleCount    = vertexInputModules.size();
                info.pPreRasterModules         = preRasterModules.data();
                info.preRasterModuleCount      = preRasterModules.size();
                info.pFragmentModules          = fragmentModules.data();
                info.fragmentModuleCount       = fragmentModules.size();
                info.pFragmentOutputModules    = fragmentOutputModules.data();
                info.fragmentOutputModuleCount = fragmentOutputModules.size();

                HF.staticResources.engineShadersLib = Create(info);
            }

            //Axis Lines Shader
            {
                ShaderModulesInfo modulesInfo{};
                modulesInfo.vertexInputModuleId    = HF.staticResources.engineShadersLibModules.quadVertexInput;
                modulesInfo.preRasterModuleId      = HF.staticResources.engineShadersLibModules.axisLinesPreRaster;
                modulesInfo.fragmentModuleId       = HF.staticResources.engineShadersLibModules.axisLinesFragment;
                modulesInfo.fragmentOutputModuleId = HF.staticResources.engineShadersLibModules.axisLinesFragmentOutput;

                ShaderCreationInfo shaderInfo{};
                shaderInfo.layout = HF.staticResources.shaderLayouts.axisLines;
                shaderInfo.library = HF.staticResources.engineShadersLib;
                shaderInfo.modules = modulesInfo;

                HF.staticResources.shaders.axisLines = Create(shaderInfo);
            }

            //Skybox Shader
            {
                ShaderModulesInfo modulesInfo{};
                modulesInfo.vertexInputModuleId    = HF.staticResources.engineShadersLibModules.defaultVertexInput;
                modulesInfo.preRasterModuleId      = HF.staticResources.engineShadersLibModules.skyboxPreRaster;
                modulesInfo.fragmentModuleId       = HF.staticResources.engineShadersLibModules.skyboxFragment;
                modulesInfo.fragmentOutputModuleId = HF.staticResources.engineShadersLibModules.skyboxFragmentOutput;

                ShaderCreationInfo shaderInfo{};
                shaderInfo.layout = HF.staticResources.shaderLayouts.skybox;
                shaderInfo.library = HF.staticResources.engineShadersLib;
                shaderInfo.modules = modulesInfo;

                HF.staticResources.shaders.skybox = Create(shaderInfo);
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