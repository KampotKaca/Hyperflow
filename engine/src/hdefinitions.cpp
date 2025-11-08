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

    VertexBufferAttribute FindVertexAttribute(const char* id)
    {
        if(!ir::HF.graphicsResources.vertexAttributes.contains(id)) log_error("Vertex Attribute not found %s", id);
        return ir::HF.graphicsResources.vertexAttributes[id];
    }
    VertexBufferAttribute FindVertexAttribute(const std::string_view id)
    {
        if(!ir::HF.graphicsResources.vertexAttributes.contains(id)) log_error("Vertex Attribute not found %s", std::string(id).c_str());
        return ir::HF.graphicsResources.vertexAttributes[id];
    }

    TextureLayout FindTextureLayout(const char* id)
    {
        if(!ir::HF.graphicsResources.textureLayouts.contains(id)) log_error("Texture Layout not found %s", id);
        return ir::HF.graphicsResources.textureLayouts[id];
    }
    TextureLayout FindTextureLayout(const std::string_view id)
    {
        if(!ir::HF.graphicsResources.textureLayouts.contains(id)) log_error("Texture Layout not found %s", std::string(id).c_str());
        return ir::HF.graphicsResources.textureLayouts[id];
    }

    TextureSampler FindTextureSampler(const char* id)
    {
        if(!ir::HF.graphicsResources.textureSamplers.contains(id)) log_error("Texture Sampler not found %s", id);
        return ir::HF.graphicsResources.textureSamplers[id];
    }
    TextureSampler FindTextureSampler(const std::string_view id)
    {
        if(!ir::HF.graphicsResources.textureSamplers.contains(id)) log_error("Texture Sampler not found %s", std::string(id).c_str());
        return ir::HF.graphicsResources.textureSamplers[id];
    }

    ShaderLayout FindShaderLayout(const char* id)
    {
        if(!ir::HF.graphicsResources.shaderLayouts.contains(id)) log_error("Shader layout not found %s", id);
        return ir::HF.graphicsResources.shaderLayouts[id];
    }
    ShaderLayout FindShaderLayout(const std::string_view id)
    {
        if(!ir::HF.graphicsResources.shaderLayouts.contains(id)) log_error("Shader layout not found %s", std::string(id).c_str());
        return ir::HF.graphicsResources.shaderLayouts[id];
    }

    Buffer FindBuffer(const char* id)
    {
        if(!ir::HF.graphicsResources.bufferIds.contains(id)) log_error("Buffer not found %s", id);
        return ir::HF.graphicsResources.bufferIds[id];
    }
    Buffer FindBuffer(const std::string_view id)
    {
        if(!ir::HF.graphicsResources.bufferIds.contains(id)) log_error("Buffer not found %s", std::string(id).c_str());
        return ir::HF.graphicsResources.bufferIds[id];
    }
}