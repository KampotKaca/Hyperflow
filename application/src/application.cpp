#include "application.h"
#include "debug.h"
#include "resources.h"
#include "voxelterrain.h"

namespace app
{
	hf::FreeMoveCamera3D freeMoveCamera{};
	hf::Camera3DAnchored anchoredCamera{};
	hf::Transform vikingRoomTransform{};
	hf::AxisLines axisLines{};
	bool drawAxisLines = true;

	void AppRendererLoad()
	{
		TextureSamplerDefineAll();
		TextureLayoutDefineAll();
		BufferAttributeDefineAll();
		UniformDefineAll();
		ShaderSetupDefineAll();
		DebugDefine();
	}

	void AppLoadResources()
	{
		TextureLoadAll();
		CubemapLoadAll();
		MeshLoadAll();
		TexturePackLoadAll();
		ShaderLoadAll();
		DebugLoad();
	}

	void AppStart()
	{
		anchoredCamera.distance = 2;
		freeMoveCamera.camera3D.position = {0, 0.5, -2};
		freeMoveCamera.camera3D.direction = {0, 0, 1};
		// freeMoveCamera.camera3D.distance = 2;
		UniformStartAll();
		DebugStart();

		VoxelTerrainGenerate();
	}

	void AppUpdate()
	{
		vikingRoomTransform.euler.y -= (float)hf::time::GetDeltaTime() * 10.0f;
		freeMoveCamera.Update(hf::GetMainWindow(), (float)hf::time::GetDeltaTime());
		DebugUpdate();

		if (hf::input::IsDown(hf::Key::N)) drawAxisLines = !drawAxisLines;
		VoxelTerrainUpdate();
	}

	void AppQuit()
	{
		VoxelTerrainDispose();
		DebugQuit();
	}

	void AppPreRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugPreRender(rn);
		UniformUploadCameraTime(rn, freeMoveCamera.camera3D.core,
			freeMoveCamera.camera3D.direction, freeMoveCamera.camera3D.position,
			freeMoveCamera.camera3D.ToViewMat4(), vikingRoomTransform.ToMat4());
	}

	void AppRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugRender(rn);

		hf::StartRenderPassPacket(rn, APP_RENDER_PASSES.mainPresentPass);
		{
			hf::StartShaderSetupPacket(rn, APP_SHADER_SETUPS.viking_room); //Viking room setup
			UniformBindCameraTime(rn);
			{
				hf::ShaderBindingInfo vikingRoomShaderInfo
				{
					.shader = APP_SHADERS.viking_room,
					.attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
					.bindingPoint = hf::RenderBindingType::Graphics
				};
				hf::StartShaderPacket(rn, vikingRoomShaderInfo);

				{
					hf::StartDrawPacket(rn);
					{
						hf::PacketAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack);
						hf::PacketAdd_DrawCall(rn, APP_MESHES.viking_room);
					}
					hf::EndDrawPacket(rn);
				}

				hf::EndShaderPacket(rn);
			}
			hf::EndShaderSetupPacket(rn);

			if (drawAxisLines)
			{
				hf::StartShaderSetupPacket(rn, APP_SHADER_SETUPS.axis_lines); //Axis lines setup
				UniformBindCameraTime(rn);
				{
					hf::ShaderBindingInfo vikingRoomShaderInfo
					{
						.shader = APP_SHADERS.axis_lines,
						.attrib = hf::resources::GetQuadBufferAttrib(),
						.bindingPoint = hf::RenderBindingType::Graphics
					};
					hf::StartShaderPacket(rn, vikingRoomShaderInfo);

					{
						axisLines.Draw(rn);
					}

					hf::EndShaderPacket(rn);
				}
				hf::EndShaderSetupPacket(rn);
			}
		}

		VoxelTerrainDraw(rn);

		hf::EndRenderPassPacket(rn);
	}
}
