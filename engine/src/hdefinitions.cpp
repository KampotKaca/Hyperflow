#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderLayout Define(const char* name, const ShaderLayoutDefinitionInfo& info)
    {
        const ShaderLayout layout = ir::HF.renderingApi.api.DefineShaderLayout(info);
        ir::HF.graphicsResources.shaderLayouts[name] = layout;
        return layout;
    }
    Buffer Define(const char* name, const BufferDefinitionInfo& info)
    {
        const Buffer buffer = ir::HF.renderingApi.api.DefineUniformBuffer(info);
        ir::HF.graphicsResources.bufferIds[name] = buffer;
        return buffer;
    }
    Buffer Define(const char* name, const StorageBufferDefinitionInfo& info)
    {
        const Buffer buffer = ir::HF.renderingApi.api.DefineStorageBuffer(info);
        ir::HF.graphicsResources.bufferIds[name] = buffer;
        return buffer;
    }

#define DEFINE_SEARCH(t, n, o)\
    const auto it = ir::HF.graphicsResources.t.find(id);\
    if (it == ir::HF.graphicsResources.t.end())\
    {\
        log_error_s("[Hyperflow] %s not found %s", n, o);\
        return 0;\
    }\
    return it->second

    VertexBufferAttribute FindVertexAttribute(const char* id) { DEFINE_SEARCH(vertexAttributes, "Vertex Attribute", id); }
    VertexBufferAttribute FindVertexAttribute(const std::string_view id) { DEFINE_SEARCH(vertexAttributes, "Vertex Attribute", std::string(id).c_str()); }

    TextureLayout FindTextureLayout(const char* id) { DEFINE_SEARCH(textureLayouts, "Texture Layout", id); }
    TextureLayout FindTextureLayout(const std::string_view id) { DEFINE_SEARCH(textureLayouts, "Texture Layout", std::string(id).c_str()); }

    TextureSampler FindTextureSampler(const char* id) { DEFINE_SEARCH(textureSamplers, "Texture Sampler", id); }
    TextureSampler FindTextureSampler(const std::string_view id) { DEFINE_SEARCH(textureSamplers, "Texture Sampler", std::string(id).c_str()); }

    ShaderLayout FindShaderLayout(const char* id) { DEFINE_SEARCH(shaderLayouts, "Shader layout", id); }
    ShaderLayout FindShaderLayout(const std::string_view id) { DEFINE_SEARCH(shaderLayouts, "Shader layout", std::string(id).c_str()); }

    Buffer FindBuffer(const char* id) { DEFINE_SEARCH(bufferIds, "Buffer", id); }
    Buffer FindBuffer(const std::string_view id) { DEFINE_SEARCH(bufferIds, "Buffer", std::string(id).c_str()); }

#undef DEFINE_SEARCH
}