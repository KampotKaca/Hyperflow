# Hyperflow

Hyperflow is a C++23 graphics engine project with various components for mesh processing, shader conversion, and core engine functionality.

## Key Features

- **Multi-API Graphics Rendering**: Supports rendering via Vulkan and Direct3D.
- **Resource Management**: Handles textures, buffers, and shaders with advanced memory allocation and cleanup mechanisms.
- **Render Pass System**: Manage subpasses, color attachments, depth attachments, and multi-sample anti-aliasing (MSAA).
- **Material and Shader Management**: Define, bind, and use complex shaders and materials in rendering pipelines.
- **Flexible Buffer System**: Includes support for vertex, index, uniform, and storage buffers with dynamic update mechanisms.
- **Texture Management**: Supports 2D textures, texture packs, and cubemaps with advanced binding and upload options.
- **Customizable Rendering Pipeline**: Fully defines how passes, shaders, and resources interact.

## Structure

The project is divided into several key components:

### 1. **Core Engine Components**
- **Main Entry Point (`main.cpp`)**: Runs the engine, passes window configuration, and program lifecycle evens, like start, update, renderer, quit etc.
- **EngineData**: Configures the main parameters for the engine, including rendering API, lifecycle callbacks, and window settings.

### 2. **Rendering API**
- Designed to abstract different graphics APIs:
    - **DirectX Implementation** (`hd3d_impl.cpp`): DirectX support is planned for the future.
    - **Vulkan Implementation** (`hvk_impl.cpp`): Vulkan-based rendering backend.

- *Features*:
    - Render pass definitions.
    - Shader creation and management.
    - Texture management.
    - Rendering frame loop control.
    - VSync and buffer synchronization.
    - Multithreaded, runtime swappable renderer vulkan to direct3d and back.

### 3. **Resources Management**
- **Textures, Shaders, Buffers** (`hresourcehandling.cpp`, `hbuffer.cpp`, etc.):
    - Handles allocation, uploading, and cleanup of graphics resources.

- **Cleanups**
    - Unused resources and buffers are marked for deletion and cleaned up systematically.

### 4. **Engine Lifecycle**
The engine lifecycle is configurable using callbacks, which are passed in main by **EngineData**:
- `RendererLoad`
- `ResourcesLoad`
- `StartCallback`
- `UpdateCallback`
- `QuitCallback`

### 5. **Renderer Lifecycle**
The renderer runs on a seperate thread, so the rendering process consists of filling the packet with necessary data and sending it to the render thread, which will draw the packet when the gpu is availble, drawing process is similar to Imgui api, for example:

        hf::Start_RenderPass(rn, APP_RENDER_PASSES.mainPresentPass);
        {
            hf::Start_ShaderLayout(rn, APP_SHADER_SETUPS.viking_room); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                hf::Start_Shader(rn, vikingRoomShaderInfo);
                {
                    hf::Start_Material(rn, APP_MATERIALS.viking_room);
                    {
                        hf::MaterialAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack, 2);
                        hf::Start_Draw(rn);
                        {
                            const VikingRoomPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoomTransform.ToMat4(),
                                .materialIndex = GetBufferIndex(APP_MATERIALS.viking_room)
                            };
                            hf::DrawSet_PushConstant(rn, &pc, sizeof(VikingRoomPushConstant));
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::End_Draw(rn);

                        hf::Start_Draw(rn);
                        {
                            const VikingRoomPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoom2Transform.ToMat4(),
                                .materialIndex = GetBufferIndex(APP_MATERIALS.viking_room)
                            };
                            hf::DrawSet_PushConstant(rn, &pc, sizeof(VikingRoomPushConstant));
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::End_Draw(rn);
                    }
                    hf::End_Material(rn);
                }
                hf::End_Shader(rn);
            }
            hf::End_ShaderLayout(rn);
            hf::skybox::Draw(rn, APP_OBJECTS.skybox);
        }
        hf::End_RenderPass(rn);

## Requirements

- C++23 compatible compiler
- CMake build system

### Prerequisites
- A Windows 10-11, or Linux (I can guarantee that app works on Arch and Windows 10, try other at your own risk) operating system.
  - CLion IDE 2025 on all platforms.
  - Linux: Cmake + Clang
  - Windows: Cmake + GCC + Ninja
- Installed Vulkan sdk on window and vulkan-devel package on arch is required.
- C++23 compiler support.

### Building the Project
### 1. Clion
      1. Clone the repository.
      2. Open the project in CLion.
      3. Configure the appropriate CMake build settings.
      4. Build and run the project.

### 2. Cmake, Linux + Clang
1. Clone the repository.
2. Change the terminal directory to the Hyperflow.
3. Run the command to create cmake build folder, 
   (On linux Clang is necessary!)
   (adding -DPRODUCTION_BUILD_DIR=Hyperflow will create stripped production ready build beside the basic cmake build folder):

       cmake -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DPRODUCTION_BUILD_DIR=Hyperflow
      
4. Build the project. (-j flag simply runs build process on multiple thread, very helpful if you want to build quickly.)  
   
       cmake --build build -j 14

### 3. Cmake, Windows + GCC
1. Clone the repository.
2. Change the terminal directory to the Hyperflow.
3. Run the command to create cmake build folder,
   (On windows gcc, g++, and ninja are necessary!)
   (adding -DPRODUCTION_BUILD_DIR=Hyperflow will create stripped production ready build beside the basic cmake build folder):

       cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -DPRODUCTION_BUILD_DIR=Hyperflow

4. Build the project. (-j flag simply runs build process on multiple thread, very helpful if you want to build quickly.)

       cmake --build build -j 14

## Known Limitations
- The Direct3D backend is currently incomplete and cannot draw frames.
- Certain advanced Vulkan features may require additional implementation.

---

## Future Enhancements
- Expand Direct3D functionality to match Vulkan's capabilities.
- Add support for additional graphics APIs, like Metal, or at least MoltenVk.
- Provide advanced debugging tools for rendering processes.
- Extend documentation and example scenes.
