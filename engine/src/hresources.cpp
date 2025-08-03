#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    namespace primitives
    {
        Ref<Mesh> GetCubeMesh() { return inter::HF.staticResources.cube; }

        Ref<VertBuffer> GetQuadBuffer()   { return inter::HF.staticResources.quadBuffer; }
        Ref<VertBuffer>* GetQuadBufferP() { return &inter::HF.staticResources.quadBuffer; }

        BufferAttrib GetQuadBufferAttrib()  { return inter::HF.staticResources.quadAttrib; }
        Buffer GetGlobalUniformBuffer()     { return inter::HF.staticResources.globalUniform; }
        Buffer GetMaterialStorageBuffer()   { return inter::HF.graphicsResources.materialDataStorageBuffer; }
        void BindGlobalUniformBuffer(const Ref<Renderer>& rn)
        {
            Start_BufferSet(rn, RenderBindingType::Graphics, 0);
            BufferSetAdd_Buffer(rn, inter::HF.staticResources.globalUniform);
            End_BufferSet(rn);
        }

        BufferAttrib GetCubeBufferAttrib() { return inter::HF.staticResources.defaultAttrib; }
        TextureSampler GetCubemapSampler() { return inter::HF.staticResources.cubemapSampler; }

        Ref<Mesh> GetCube()      { return inter::HF.staticResources.cube; }
        Ref<Mesh> GetPlane()     { return inter::HF.staticResources.plane; }
        Ref<Mesh> GetIcoSphere() { return inter::HF.staticResources.icoSphere; }
        Ref<Mesh> GetUvSphere()  { return inter::HF.staticResources.uvSphere; }

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
                formats[0] = BufferAttribFormat{ .type = BufferDataType::F32, .size = 2 }; //Position

                BufferAttribDefinitionInfo attribInfo{};
                attribInfo.bindingId = 0;
                attribInfo.formatCount = formats.size();
                attribInfo.pFormats = formats.data();

                HF.staticResources.quadAttrib = Define(attribInfo);
            }

            //Cube Attrib
            {
                std::array<BufferAttribFormat, 3> formats{};
                formats[0] = BufferAttribFormat{ .type = BufferDataType::F32, .size = 3 }; //Position
                formats[1] = BufferAttribFormat{ .type = BufferDataType::F32, .size = 3 }; //Normal
                formats[2] = BufferAttribFormat{ .type = BufferDataType::F32, .size = 2 }; //TexCoord

                BufferAttribDefinitionInfo attribInfo{};
                attribInfo.bindingId = 0;
                attribInfo.formatCount = formats.size();
                attribInfo.pFormats = formats.data();

                HF.staticResources.defaultAttrib = Define(attribInfo);
            }
        }

        void DefineTextureLayouts()
        {
            //Skybox Texture Layout
            {
                TextureLayoutBindingInfo cubemapBinding{};
                cubemapBinding.bindingId = 0;
                cubemapBinding.usageFlags = ShaderUsageStage::Vertex |
                                            ShaderUsageStage::Fragment;
                cubemapBinding.arraySize = 1;

                TextureLayoutDefinitionInfo layoutInfo{};
                layoutInfo.pBindings = &cubemapBinding;
                layoutInfo.bindingCount = 1;

                HF.staticResources.skyboxLayout = Define(layoutInfo);
            }
        }

        void DefineTextureSamplers()
        {
            //Cubemap Sampler
            {
                TextureSamplerDefinitionInfo samplerInfo{};
                samplerInfo.comparison = ComparisonOperation::Never;
                samplerInfo.repeatMode = TextureRepeatMode::ClampToEdge;

                HF.staticResources.cubemapSampler = Define(samplerInfo);
            }
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
                bindingInfo.usageFlags = ShaderUsageStage::All;
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
                pushConstantInfo.usageFlags = ShaderUsageStage::Vertex |
                                              ShaderUsageStage::Fragment;
                pushConstantInfo.sizeInBytes = sizeof(GridLinesInfo);

                ShaderLayoutDefinitionInfo info{};
                info.pushConstant = pushConstantInfo;
                info.pBuffers = &HF.staticResources.globalUniform;
                info.bufferCount = 1;

                HF.staticResources.axisLinesShaderLayout = Define(info);
            }

            //Skybox Shader Setup
            {
                PushConstantInfo pushConstantInfo{};
                pushConstantInfo.usageFlags = ShaderUsageStage::Vertex |
                                              ShaderUsageStage::Fragment;
                pushConstantInfo.sizeInBytes = 0;

                ShaderLayoutDefinitionInfo info{};
                info.pushConstant = pushConstantInfo;
                info.pBuffers = &HF.staticResources.globalUniform;
                info.bufferCount = 1;
                info.pTextureLayouts = &HF.staticResources.skyboxLayout;
                info.textureLayoutCount = 1;

                HF.staticResources.skyboxShaderLayout = Define(info);
            }
        }

        void LoadCubemaps()
        {
            //Skybox cubemap
            {
                CubemapTexturePaths texPaths{};
                texPaths.left  = "left.png";
                texPaths.right = "right.png";
                texPaths.down  = "down.png";
                texPaths.up    = "up.png";
                texPaths.back  = "back.png";
                texPaths.front = "front.png";

                CubemapCreationInfo info{};
                info.folderPath = "__toony";
                info.desiredChannel = TextureChannel::RGBA;
                info.mipLevels = 1;
                info.texturePaths = texPaths;

                HF.staticResources.defaultSkyboxCubemap = Create(info);
                HF.staticResources.boundCubemap = HF.staticResources.defaultSkyboxCubemap;
            }
        }

        void LoadTexturePacks()
        {
            //Skybox texturepack
            {
                TexturePackBindingInfo<Cubemap>::TextureInfo ti{};
                ti.texture = HF.staticResources.defaultSkyboxCubemap;
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

                HF.staticResources.skyboxTexturePack = Create(info);
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

                VertBufferCreationInfo bufferInfo{};
                bufferInfo.bufferAttrib = HF.staticResources.quadAttrib;
                bufferInfo.memoryType = BufferMemoryType::Static;
                bufferInfo.usageFlags = BufferUsageType::All;
                bufferInfo.vertexCount = 6;
                bufferInfo.pVertices = vertices;

                HF.staticResources.quadBuffer = Create(bufferInfo);
            }

            MeshStats primitiveStats{};
            primitiveStats.typeFlags = MeshDataType::Position |
                                       MeshDataType::Normal |
                                       MeshDataType::TexCoord;
            primitiveStats.memoryType = BufferMemoryType::Static;
            primitiveStats.bufferAttrib = HF.staticResources.defaultAttrib;

            //Cube Mesh
            {
                MeshCreationInfo info{};
                info.filePath = "__cube.obj";
                info.stats = primitiveStats;

                HF.staticResources.cube = Create(info);
            }

            //Plane mesh
            {
                MeshCreationInfo info{};
                info.filePath = "__plane.obj";
                info.stats = primitiveStats;

                HF.staticResources.plane = Create(info);
            }

            //Ico Sphere mesh
            {
                MeshCreationInfo info{};
                info.filePath = "__ico_sphere.obj";
                info.stats = primitiveStats;

                HF.staticResources.icoSphere = Create(info);
            }

            //Uv Sphere mesh
            {
                MeshCreationInfo info{};
                info.filePath = "__uv_sphere.obj";
                info.stats = primitiveStats;

                HF.staticResources.uvSphere = Create(info);
            }
        }

        void LoadShaders()
        {
            //Shader Library!
            {
                //--------- Vertex Input ------------------------------------------------
                ShaderLibraryVertexInputModuleInfo quadVertInputModule{};
                quadVertInputModule.pAttributes[0] = HF.staticResources.quadAttrib;
                quadVertInputModule.attributeCount = 1;

                ShaderLibraryVertexInputModuleInfo defaultVertInputModule{};
                defaultVertInputModule.pAttributes[0] = HF.staticResources.defaultAttrib;
                defaultVertInputModule.attributeCount = 1;

                std::array vertexInputModules
                {
                    ShaderLibraryModule
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.quadVertexInput,
                        .module = quadVertInputModule,
                    },
                    ShaderLibraryModule
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.defaultVertexInput,
                        .module = defaultVertInputModule,
                    },
                };

                //--------- Pre Raster ------------------------------------------------

                ShaderDepthBiasOptions axisLinesDepthBiasOptions{};
                axisLinesDepthBiasOptions.constantFactor = 1.25f;
                axisLinesDepthBiasOptions.slopeFactor = 1.75f;

                ShaderRasterizerOptions axisLinesRasterOptions{};
                axisLinesRasterOptions.cullMode = ShaderCullMode::None;
                axisLinesRasterOptions.biasOptions = axisLinesDepthBiasOptions;

                ShaderRasterizerOptions skyboxRasterOptions{};
                skyboxRasterOptions.cullMode = ShaderCullMode::Front;

                ShaderLibraryPreRasterModuleInfo axisLinesPreRasterModule{};
                axisLinesPreRasterModule.vertexShaderPath = FilePath{ .path = "__axislines" };
                axisLinesPreRasterModule.options = axisLinesRasterOptions;
                axisLinesPreRasterModule.layout = HF.staticResources.axisLinesShaderLayout;

                ShaderLibraryPreRasterModuleInfo skyboxPreRasterModule{};
                skyboxPreRasterModule.vertexShaderPath = FilePath{ .path = "__skybox" };
                skyboxPreRasterModule.options = skyboxRasterOptions;
                skyboxPreRasterModule.layout = HF.staticResources.skyboxShaderLayout;

                std::array preRasterModules
                {
                    ShaderLibraryModule //Axis Lines Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesPreRaster,
                        .module = axisLinesPreRasterModule
                    },
                    ShaderLibraryModule //Skybox Vertex Shader
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxPreRaster,
                        .module = skyboxPreRasterModule
                    },
                };

                //--------- Fragment ------------------------------------------------

                ShaderDepthStencilOptions axisLinesDepthStencilOptions{};
                axisLinesDepthStencilOptions.enableDepth = true;
                axisLinesDepthStencilOptions.writeDepth = true;
                axisLinesDepthStencilOptions.comparisonFunc = DepthComparisonFunction::LessOrEqual;
                axisLinesDepthStencilOptions.enableDepthBounds = false;
                axisLinesDepthStencilOptions.enableStencil = false;

                ShaderDepthStencilOptions skyboxDepthStencilOptions{};
                skyboxDepthStencilOptions.enableDepth = true;
                skyboxDepthStencilOptions.writeDepth = false;
                skyboxDepthStencilOptions.comparisonFunc = DepthComparisonFunction::Less;
                skyboxDepthStencilOptions.enableDepthBounds = false;
                skyboxDepthStencilOptions.enableStencil = false;

                ShaderLibraryFragmentModuleInfo axisLinesFragmentModule{};
                axisLinesFragmentModule.fragmentShaderPath = FilePath{ .path = "__axislines" };
                axisLinesFragmentModule.depthStencilOptions = axisLinesDepthStencilOptions;
                axisLinesFragmentModule.layout = HF.staticResources.axisLinesShaderLayout;

                ShaderLibraryFragmentModuleInfo skyboxFragmentModule{};
                skyboxFragmentModule.fragmentShaderPath = FilePath{ .path = "__skybox" };
                skyboxFragmentModule.depthStencilOptions = skyboxDepthStencilOptions;
                skyboxFragmentModule.layout = HF.staticResources.skyboxShaderLayout;

                std::array fragmentModules
                {
                    ShaderLibraryModule
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragment,
                        .module = axisLinesFragmentModule
                    },
                    ShaderLibraryModule
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragment,
                        .module = skyboxFragmentModule
                    },
                };

                //--------- Fragment Output ------------------------------------------------

                ColorAttachmentSettings colorAttachmentSettings{};
                colorAttachmentSettings.colorWriteMask = ColorMaskingFlags::All;
                colorAttachmentSettings.blending = ShaderBlendingOptions{};

                ShaderLibraryFragmentOutputModuleInfo axisLinesFragmentOutputModule{};
                axisLinesFragmentOutputModule.pColorAttachmentsSettings[0] = colorAttachmentSettings;
                axisLinesFragmentOutputModule.colorAttachmentCount = 1;

                ShaderLibraryFragmentOutputModuleInfo skyboxFragmentOutputModule{};
                skyboxFragmentOutputModule.colorAttachmentCount = 1;

                std::array fragmentOutputModules
                {
                    ShaderLibraryModule //Axis lines
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.axisLinesFragmentOutput,
                        .module = axisLinesFragmentOutputModule
                    },
                    ShaderLibraryModule //Skybox
                    {
                        .resultId = &HF.staticResources.engineShadersLibModules.skyboxFragmentOutput,
                        .module = skyboxFragmentOutputModule
                    },
                };

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
                shaderInfo.layout = HF.staticResources.axisLinesShaderLayout;
                shaderInfo.library = HF.staticResources.engineShadersLib;
                shaderInfo.modules = modulesInfo;

                HF.staticResources.axisLinesShader = Create(shaderInfo);
            }

            //Skybox Shader
            {
                ShaderModulesInfo modulesInfo{};
                modulesInfo.vertexInputModuleId    = HF.staticResources.engineShadersLibModules.defaultVertexInput;
                modulesInfo.preRasterModuleId      = HF.staticResources.engineShadersLibModules.skyboxPreRaster;
                modulesInfo.fragmentModuleId       = HF.staticResources.engineShadersLibModules.skyboxFragment;
                modulesInfo.fragmentOutputModuleId = HF.staticResources.engineShadersLibModules.skyboxFragmentOutput;

                ShaderCreationInfo shaderInfo{};
                shaderInfo.layout = HF.staticResources.skyboxShaderLayout;
                shaderInfo.library = HF.staticResources.engineShadersLib;
                shaderInfo.modules = modulesInfo;

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