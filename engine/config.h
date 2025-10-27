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

#define RENDERING_MAX_NUM_RENDER_TEXTURES 16
#define MAX_COLOR_ATTACHMENTS 4
#define MAX_TEXTURES_IN_TEXTURE_PACK 8
#define MAX_TEXTURES_IN_TEXTURE_ARRAY 8
#define MAX_OBJECT_BINDINGS 8

//Vulkan
#define VK_MSAA_MIN_SAMPLE_SHADING 0.2f

#define VK_MAX_PUSH_CONSTANT_RANGES 8

#define VK_MAX_STORAGE_BUFFER_DESCRIPTORS 64
#define VK_MAX_UNIFORM_BUFFER_DESCRIPTORS 128
#define VK_MAX_COMBINED_IMAGE_SAMPLER_DESCRIPTORS 256

#define VULKAN_API_VERSION VK_MAKE_VERSION(1, 1, 0)
#define VULKAN_API_COLOR_FORMAT TextureFormat::R8G8B8A8_Unorm
#define VULKAN_API_MAX_COMMANDS_PER_FRAME 8
#define VULKAN_API_MAX_TIMEOUT 20000000000 //20seconds
#define VULKAN_API_MAX_NUM_COPY_REGIONS 8

#define MAX_NUM_BUFFER_CACHE 16
#define MAX_NUM_BOUND_DESCRIPTORS 16

#define MAX_AUDIO_LISTENERS 4
#define MAX_VERTEX_INPUT_BUFFER_ATTRIBUTES 4

#define MAX_PER_INSTANCE_BUFFER_SIZE 128
#define MAX_INSTANCE_BUFFER_SIZE_BYTES (MAX_PER_INSTANCE_BUFFER_SIZE * 8 * 1024 * 32)

#define MAX_DEFORMATIONS_PER_VERTEX 4

#define HF_ENABLE_ALLOCATOR
// #define VK_ENABLE_PIPELINE_CACHES

#if defined(DEBUG)
#define VK_ENABLE_VALIDATION
// #define VK_ENABLE_DEEP_VALIDATION
#endif

//------------------------------ Engine Preallocations -----------------------------------------------------
//These numbers are suggestions if the size exceeds, program will still work,
//but this will call allocation at runtime, so this is not optimal.
#define EN_NUM_RESOURCES_MARKED_FOR_DELETION 32
#define EN_NUM_WINDOWS                       8


//------------------------------ Renderer Preallocations ---------------------------------------------------
//These numbers are suggestions if the size exceeds, program will still work,
//but this will call allocations at runtime, so this is not optimal.
#define RN_NUM_DIRECTIONAL_LIGHTS 8
#define RN_NUM_SPOT_LIGHTS        8
#define RN_NUM_POINT_LIGHTS       8

#define RN_NUM_RENDER_ATTACHMENT_DEPENDENCIES 8
#define RN_NUM_RENDER_TEXTURES                8
#define RN_NUM_SHADER_LAYOUTS                 64
#define RN_NUM_SHADERS                        128
#define RN_NUM_MATERIALS                      256
#define RN_NUM_DRAW_PACKETS                   512
#define RN_NUM_TEX_PACKS                      2 * RN_NUM_DRAW_PACKETS

#define RN_NUM_DRAW_CALLS                     4 * RN_NUM_DRAW_PACKETS
#define RN_NUM_DRAW_INSTANCES                 4 * RN_NUM_DRAW_CALLS

#define RN_NUM_DRAW_INSTANCES                 4 * RN_NUM_DRAW_CALLS
#define RN_NUM_VERTEX_BUFFERS                 2 * RN_NUM_DRAW_CALLS

#define RN_NUM_BUFFERS_SETS                   8
#define RN_NUM_BUFFERS                        4 * RN_NUM_BUFFERS_SETS

#define RN_NUM_BUFFER_UPLOAD_DATA             256 * RN_NUM_BUFFERS
#define RN_NUM_PUSH_CONSTANT_UPLOAD_DATA      128 * RN_NUM_DRAW_CALLS
#define RN_NUM_INSTANCE_UPLOAD_DATA           64 * RN_NUM_DRAW_INSTANCES

#define RN_NUM_TEX_PACK_GROUP_UPLOADS         8
#define RN_NUM_TEX_PACK_UPLOADS               4 * RN_NUM_TEX_PACK_GROUP_UPLOADS
#define RN_NUM_TEXTURE_UPLOADS                4 * RN_NUM_TEX_PACK_UPLOADS
#define RN_NUM_BUFFER_UPLOADS                 32


#define EN_NUM_COMMANDS_COMMAND_POOL          8

#endif //CONFIG_H
