#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderLayout Define(const char* name, const ShaderLayoutDefinitionInfo& info)
    {
        const ShaderLayout layout = inter::HF.renderingApi.api.DefineShaderLayout(info);
        inter::HF.graphicsResources.shaderLayouts[name] = layout;
        return layout;
    }

    Buffer Define(const char* name, const BufferDefinitionInfo& info)
    {
        const Buffer buffer = inter::HF.renderingApi.api.DefineUniformBuffer(info);
        inter::HF.graphicsResources.bufferIds[name] = buffer;
        return buffer;
    }
    Buffer Define(const char* name, const StorageBufferDefinitionInfo& info)
    {
        const Buffer buffer = inter::HF.renderingApi.api.DefineStorageBuffer(info);
        inter::HF.graphicsResources.bufferIds[name] = buffer;
        return buffer;
    }

    VertexBufferAttribute FindVertexAttribute(const char* id)
    {
        if(!inter::HF.graphicsResources.vertexAttributes.contains(id)) LOG_ERROR("Vertex Attribute not found %s", id);
        return inter::HF.graphicsResources.vertexAttributes[id];
    }
    VertexBufferAttribute FindVertexAttribute(const std::string_view id)
    {
        if(!inter::HF.graphicsResources.vertexAttributes.contains(id)) LOG_ERROR("Vertex Attribute not found %s", std::string(id).c_str());
        return inter::HF.graphicsResources.vertexAttributes[id];
    }

    TextureLayout FindTextureLayout(const char* id)
    {
        if(!inter::HF.graphicsResources.textureLayouts.contains(id)) LOG_ERROR("Texture Layout not found %s", id);
        return inter::HF.graphicsResources.textureLayouts[id];
    }
    TextureLayout FindTextureLayout(const std::string_view id)
    {
        if(!inter::HF.graphicsResources.textureLayouts.contains(id)) LOG_ERROR("Texture Layout not found %s", std::string(id).c_str());
        return inter::HF.graphicsResources.textureLayouts[id];
    }

    TextureSampler FindTextureSampler(const char* id)
    {
        if(!inter::HF.graphicsResources.textureSamplers.contains(id)) LOG_ERROR("Texture Sampler not found %s", id);
        return inter::HF.graphicsResources.textureSamplers[id];
    }
    TextureSampler FindTextureSampler(const std::string_view id)
    {
        if(!inter::HF.graphicsResources.textureSamplers.contains(id)) LOG_ERROR("Texture Sampler not found %s", std::string(id).c_str());
        return inter::HF.graphicsResources.textureSamplers[id];
    }

    ShaderLayout FindShaderLayout(const char* id)
    {
        if(!inter::HF.graphicsResources.shaderLayouts.contains(id)) LOG_ERROR("Shader layout not found %s", id);
        return inter::HF.graphicsResources.shaderLayouts[id];
    }
    ShaderLayout FindShaderLayout(const std::string_view id)
    {
        if(!inter::HF.graphicsResources.shaderLayouts.contains(id)) LOG_ERROR("Shader layout not found %s", std::string(id).c_str());
        return inter::HF.graphicsResources.shaderLayouts[id];
    }

    Buffer FindBuffer(const char* id)
    {
        if(!inter::HF.graphicsResources.bufferIds.contains(id)) LOG_ERROR("Buffer not found %s", id);
        return inter::HF.graphicsResources.bufferIds[id];
    }
    Buffer FindBuffer(const std::string_view id)
    {
        if(!inter::HF.graphicsResources.bufferIds.contains(id)) LOG_ERROR("Buffer not found %s", std::string(id).c_str());
        return inter::HF.graphicsResources.bufferIds[id];
    }
}