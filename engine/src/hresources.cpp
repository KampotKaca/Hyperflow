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

        Ref<Mesh> GetMesh(PrimitiveMeshType type) { return GetMesh(inter::HF.staticResources.primitiveModels[(uint32_t)type], 0); }
        Ref<Texture> GetTexture(PrimitiveTextureType type) { return inter::HF.staticResources.primitiveTextures[(uint32_t)type]; }
    }

    namespace inter::general
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
                BufferBindingInfo info{};
                info.usageFlags = ShaderUsageStageFlags::All;
                info.arraySize = 1;
                info.elementSizeInBytes = sizeof(GlobalUniformInfo);

                BufferDefinitionInfo uniform{};
                uniform.bindingId = 0;
                uniform.pBindings = &info;
                uniform.bindingCount = 1;

                HF.staticResources.globalUniform = Define("__global_uniform", uniform);
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

                HF.graphicsResources.materialDataStorageBuffer = Define("__material_storage", info);
            }

            SubmitAllBuffers();
        }

        void DefineShaderLayouts()
        {
            //Axis Lines Shader Layout
            {
                PushConstantInfo pushConstantInfo{};
                pushConstantInfo.usageFlags = ShaderUsageStageFlags::Vertex |
                                              ShaderUsageStageFlags::Fragment;
                pushConstantInfo.sizeInBytes = sizeof(GridLinesInfo);

                ShaderLayoutDefinitionInfo info{};
                info.pushConstant = pushConstantInfo;
                info.pBuffers = &HF.staticResources.globalUniform;
                info.bufferCount = 1;

                HF.staticResources.shaderLayouts.axisLines = Define("__axislines", info);
            }

            //Skybox Shader Layout
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

                HF.staticResources.shaderLayouts.skybox = Define("__skybox", info);
            }
        }

        void LoadCubemaps()
        {
            HF.staticResources.skyboxResources.defaultCubemap = Cast<Cubemap>(CreateAsset("__toony", AssetType::Cubemap));
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
                    vec2{ -0.5, -0.5 }, vec2{ 0.5, 0.5 },  vec2{ -0.5, 0.5 },
                    vec2{ -0.5, -0.5 }, vec2{ 0.5, -0.5 }, vec2{ 0.5, 0.5 },
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

            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::Cube]      = Cast<Model>(CreateAsset("__cube.obj",       AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::IcoSphere] = Cast<Model>(CreateAsset("__ico_sphere.obj", AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::Plane]     = Cast<Model>(CreateAsset("__plane.obj",      AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::UVSphere]  = Cast<Model>(CreateAsset("__uv_sphere.obj",  AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::Cone]      = Cast<Model>(CreateAsset("__cone.obj",       AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::Cylinder]  = Cast<Model>(CreateAsset("__cylinder.obj",   AssetType::Model));
            HF.staticResources.primitiveModels[(uint32_t)PrimitiveMeshType::Torus]     = Cast<Model>(CreateAsset("__torus.obj",      AssetType::Model));
        }

        void LoadTextures()
        {
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Default]           = Cast<Texture>(CreateAsset("__default.png",             AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Circle]            = Cast<Texture>(CreateAsset("__circle.png",              AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Triangle]          = Cast<Texture>(CreateAsset("__triangle.png",            AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::Capsule]           = Cast<Texture>(CreateAsset("__capsule.png",             AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::HexagonFlatTop]    = Cast<Texture>(CreateAsset("__hexagon_flat_top.png",    AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::HexagonPointedTop] = Cast<Texture>(CreateAsset("__hexagon_pointed_top.png", AssetType::Texture));
            HF.staticResources.primitiveTextures[(uint32_t)PrimitiveTextureType::IsometricDiamond]  = Cast<Texture>(CreateAsset("__isometric_diamond.png",   AssetType::Texture));
        }

        void LoadShaders()
        {
            HF.staticResources.engineShadersLib = hf::Cast<ShaderLibrary>(CreateAsset("__engine_shader_lib/__engine_shader_lib", AssetType::ShaderLibrary));
            HF.staticResources.shaders.axisLines = hf::Cast<Shader>(CreateAsset("__axislines", AssetType::Shader));
            HF.staticResources.shaders.skybox = hf::Cast<Shader>(CreateAsset("__skybox", AssetType::Shader));
        }
    }
}