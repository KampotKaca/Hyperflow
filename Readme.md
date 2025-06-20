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
- **Main Entry Point (`main.cpp`)**: Runs the engine, passes window configuration, and program lifecycle evens, like start, update, renderer, quit and etc.
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
    - Multithreaded, runtime swappable renderer vulkan -> direct3d and back.

### 3. **Resources Management**
- **Textures, Shaders, Buffers** (`hresourcehandling.cpp`, `hbuffer.cpp`, etc.):
    - Handles allocation, uploading, and cleanup of graphics resources.

- **Cleanups**
    - Unused resources and buffers are marked for deletion and cleaned up systematically.

### 4. **Engine Lifecycle**
The graphics engine lifecycle is configurable using callbacks:
- `RendererLoad`
- `ResourcesLoad`
- `StartCallback`
- `UpdateCallback`
- `QuitCallback`

### 5. **Renderer Lifecycle**
The renderer runs on a seperate thread, so the rendering process consists of filling the packet with necessary data and sending it to the render thread, which will draw the packet when the gpu is availble, drawing process is similar to Imgui api, for example:

        rn->Start_RenderPass(APP_RENDER_PASSES.mainPresentPass);
        {
            rn->Start_ShaderSetup(APP_SHADER_SETUPS.viking_room); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                rn->Start_Shader(vikingRoomShaderInfo);
                {
                    rn->Start_Material(hf::primitives::GetEmptyMaterial());
                    {
                        rn->MaterialAdd_TexturePackBinding(APP_TEXTURE_PACKS.viking_room_pack, 1);
                        rn->Start_Draw();
                        {
                            const auto trs = APP_OBJECTS.vikingRoomTransform.ToMat4();
                            rn->DrawSet_PushConstant(&trs, sizeof(trs));
                            rn->DrawAdd_DrawCall(APP_MESHES.viking_room);
                        }
                        rn->End_Draw();

                        rn->Start_Draw();
                        {
                            const auto trs = APP_OBJECTS.vikingRoom2Transform.ToMat4();
                            rn->DrawSet_PushConstant(&trs, sizeof(trs));
                            rn->DrawAdd_DrawCall(APP_MESHES.viking_room);
                        }
                        rn->End_Draw();
                    }
                    rn->End_Material();
                }
                rn->End_Shader();
            }
            rn->End_ShaderSetup();
            hf::skybox::Draw(rn, APP_OBJECTS.skybox);
        }
        rn->End_RenderPass();

## Requirements

- C++23 compatible compiler
- CMake build system

### Prerequisites
- A Windows 10 operating system.
- CLion IDE 2025.1.2 or compatible build system.
- Installed Vulkan and/or Direct3D runtime libraries.
- C++23 compiler support.

### Building the Project
1. Clone the repository.
2. Open the project in CLion.
3. Configure the appropriate CMake build settings.
4. Build and run the project.

## Known Limitations
- The Direct3D backend is currently incomplete and cannot draw frames.
- Certain advanced Vulkan features may require additional implementation.

---

## Future Enhancements
- Expand Direct3D functionality to match Vulkan's capabilities.
- Add support for additional graphics APIs, like Metal.
- Provide advanced debugging tools for rendering processes.
- Extend documentation and example scenes.
