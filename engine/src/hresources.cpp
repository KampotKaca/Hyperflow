#include "hinternal.h"
#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    namespace primitives
    {
        BufferAttrib GetQuadBufferAttrib() { return inter::HF.staticResources.quadAttrib; }
        TextureLayout GetEmptyTextureLayout() { return inter::HF.staticResources.emptyLayout; }

        BufferAttrib GetCubeBufferAttrib() { return inter::HF.staticResources.cubeAttrib; }
        TextureSampler GetCubemapSampler() { return inter::HF.staticResources.cubemapSampler; }
        UniformBuffer GetCameraUniform() { return inter::HF.staticResources.cameraUniform; }

        Ref<Mesh> GetCube() { return inter::HF.staticResources.cube; }
        Ref<Material> GetEmptyMaterial() { return inter::HF.staticResources.emptyMaterial; }
    }

    namespace inter::primitives
    {
        static void DefineTextureLayouts();
        static void DefineBufferAttribs();
        static void DefineTextureSamplers();
        static void DefineUniforms();
        static void DefineShaderSetups();

        static void LoadMeshes();
        static void LoadShaders();
        static void LoadMaterials();

        void DefineStaticResources_i()
        {
            DefineTextureLayouts();
            DefineBufferAttribs();
            DefineTextureSamplers();
            DefineUniforms();
            DefineShaderSetups();
        }

        void LoadStaticResources_i()
        {
            LoadShaders();
            LoadMeshes();
            LoadMaterials();
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
                        .sizeInBytes = sizeof(AxisLines::UploadInfo),
                    },
                    .pBuffers = &HF.staticResources.cameraUniform,
                    .bufferCount = 1,
                    .pTextureLayouts = &HF.staticResources.emptyLayout,
                    .textureLayoutCount = 1
                };

                HF.staticResources.axisLinesShaderSetup = shadersetup::Define(info);
            }
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

                HF.staticResources.quadAttrib = bufferattrib::Define(attribInfo);
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

                HF.staticResources.cubeAttrib = bufferattrib::Define(attribInfo);
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

                HF.staticResources.emptyLayout = texturelayout::Define(layoutInfo);
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

                HF.staticResources.cubemapSampler = texturesampler::Define(samplerInfo);
            }
        }

        void DefineUniforms()
        {
            //Camera uniform
            {
                UniformBufferBindingInfo bindingInfo
                {
                    .usageFlags = ShaderUsageStage::Default,
                    .arraySize = 1,
                    .elementSizeInBytes = sizeof(CameraUniform)
                };

                const UniformBufferDefinitionInfo info
                {
                    .bindingId = 0,
                    .pBindings = &bindingInfo,
                    .bindingCount = 1
                };

                HF.staticResources.cameraUniform = uniformbuffer::Define(info);
            }

            //allocator
            {
                std::array uniforms{ HF.staticResources.cameraUniform };

                const UniformAllocatorDefinitionInfo info
                {
                    .pBuffers = uniforms.data(),
                    .bufferCount = uniforms.size(),
                };

                HF.staticResources.uniformAllocator = uniformallocator::Define(info);
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

                HF.staticResources.quadBuffer = vertbuffer::Create(bufferInfo);
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
                HF.staticResources.cube = mesh::Create(info);
            }
        }

        void LoadShaders()
        {
            //Axis Lines Shader
            {
                auto quadAttrib = hf::primitives::GetQuadBufferAttrib();
                ShaderCreationInfo shaderInfo
                {
                    .renderPass = HF.mainWindow->renderer->mainPass,
                    .setup = HF.staticResources.axisLinesShaderSetup,
                    .supportedAttribCount = 1,
                    .pSupportedAttribs = &quadAttrib,
                    .vertexShaderLoc = "__axislines",
                    .fragmentShaderLoc = "__axislines",
                    .alphaTestOptions =
                    {
                        .blendMode = ShaderBlendMode::Alpha,
                        .blendOp = ShaderBlendOp::XOr
                    },
                    .depthStencilOptions =
                    {
                        .enableDepth = false,
                        .writeDepth = false,
                        .comparisonFunc = DepthComparisonFunction::LessOrEqual,
                        .enableDepthBounds = false,
                        .enableStencil = false,
                    }
                };
                HF.staticResources.axisLinesShader = shader::Create(shaderInfo);
            }
        }

        void LoadMaterials()
        {
            const MaterialCreationInfo materialInfo
            {
                .sizeInBytes = 0
            };
            HF.staticResources.emptyMaterial = material::Create(materialInfo);
        }
    }
}