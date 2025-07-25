#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.0.0"
#define HF_ENGINE_TITLE "Hyperflow"
#define FRAMES_IN_FLIGHT 2

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 8

//maximum size allocated memory for a material
#define RENDERING_MAX_MATERIAL_MEMORY_BADGET 1024
//each octree consists of 4096 materials
#define RENDERING_MAX_MATERIAL_OCTREE_COUNT 1
//change this value at your own risk!
#define RENDERING_MAX_PUSH_CONSTANT_SIZE 128

#define RENDERING_MAX_UNIFORM_UPLOAD_COUNT 256
#define RENDERING_MAX_UNIFORM_UPLOAD_BUFFER_SIZE (4096 * 4)
#define RENDERING_MAX_PUSH_CONSTANT_UPLOAD_BUFFER_SIZE (RENDERING_MAX_NUM_SHADER_LAYOUTS * 1024)
#define RENDERING_MAX_NUM_DRAW_CALLS 1024
#define RENDERING_MAX_NUM_TEXPACKS 1024
#define RENDERING_MAX_NUM_TEXPACK_REBINDING 128
#define RENDERING_MAX_NUM_TEXTURES 128
#define RENDERING_MAX_NUM_UNIFORMS 4096
#define RENDERING_MAX_NUM_STORAGE_BUFFERS 256
#define RENDERING_MAX_NUM_DRAWPACKETS 1024
#define RENDERING_MAX_NUM_MATERIALS 256
#define RENDERING_MAX_NUM_SHADERS 256
#define RENDERING_MAX_NUM_SHADER_LAYOUTS 128
#define RENDERING_MAX_NUM_RENDER_ATTACHMENT_DEPENDENCIES 16
#define RENDERING_MAX_NUM_RENDER_TEXTURES 16

#define MAX_COLOR_ATTACHMENTS 8
#define MAX_TEXTURES_IN_TEXTURE_PACK 8
#define MAX_TEXTURES_IN_TEXTURE_ARRAY 8

//Vulkan
#define VK_MSAA_MIN_SAMPLE_SHADING 0.2f

#define VK_MAX_INDICES 4096
#define VK_MAX_UNIFORM_AND_TEXTURE_UPLOADS 1024
#define VK_MAX_UNIFORM_AND_TEXTURE_BINDINGS 128
#define VK_MAX_IMAGE_BINDINGS 512
#define VK_MAX_IMAGE_BARRIERS 512
#define VK_MAX_PUSH_CONSTANT_RANGES 8

#define VULKAN_API_VERSION VK_MAKE_VERSION(1, 1, 0)
#define VULKAN_API_COLOR_FORMAT TextureFormat::R8G8B8A8_Unorm
#define VULKAN_API_MAX_COMMANDS_PER_FRAME 8
#define VULKAN_API_MAX_TIMEOUT 20000000000 //20seconds
#define VULKAN_API_MAX_NUM_COPY_REGIONS 8

#define MAX_NUM_BUFFER_CACHE 16
#define MAX_NUM_BOUND_DESCRIPTORS 16

#define MAX_AUDIO_LISTENERS 4

#endif //CONFIG_H
