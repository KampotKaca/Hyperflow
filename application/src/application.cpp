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
		MaterialLoadAll();
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

		// VoxelTerrainGenerate();
		hf::time::SetTargetFrameRate(-1);
	}

	void AppUpdate()
	{
		vikingRoomTransform.euler.y -= (float)hf::time::GetDeltaTime() * 10.0f;
		freeMoveCamera.Update(hf::GetMainWindow(), (float)hf::time::GetDeltaTime());
		DebugUpdate();

		if (hf::input::IsDown(hf::Key::N)) drawAxisLines = !drawAxisLines;
		// VoxelTerrainUpdate();
	}

	void AppQuit()
	{
		// VoxelTerrainDispose();
		DebugQuit();
	}

	void AppPreRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugPreRender(rn);
	}

	void AppRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugRender(rn);

		UniformUploadCamera(rn, freeMoveCamera.camera3D.core,
			freeMoveCamera.camera3D.direction, freeMoveCamera.camera3D.position,
			freeMoveCamera.camera3D.ToViewMat4());
		UniformUploadTime(rn);

		hf::draw::StartRenderPassPacket(rn, APP_RENDER_PASSES.mainPresentPass);
		{
			hf::draw::StartShaderSetupPacket(rn, APP_SHADER_SETUPS.viking_room); //Viking room setup
			UniformBindCamera(rn);
			UniformBindTime(rn);
			{
				const hf::ShaderBindingInfo vikingRoomShaderInfo
				{
					.shader = APP_SHADERS.viking_room,
					.attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
					.bindingPoint = hf::RenderBindingType::Graphics
				};
				hf::draw::StartShaderPacket(rn, vikingRoomShaderInfo);
				{
					hf::draw::StartMaterialPacket(rn, APP_MATERIALS.viking_room);
					{
						hf::draw::StartDrawPacket(rn);
						{
							const auto trs = vikingRoomTransform.ToMat4();
							hf::draw::PacketAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack);
							hf::draw::PacketSet_PushConstant(rn, &trs, sizeof(trs));
							hf::draw::PacketAdd_DrawCall(rn, APP_MESHES.viking_room);
						}
						hf::draw::EndDrawPacket(rn);
					}
					hf::draw::EndMaterialPacket(rn);
				}
				hf::draw::EndShaderPacket(rn);
			}
			hf::draw::EndShaderSetupPacket(rn);

			if (drawAxisLines)
			{
				hf::draw::StartShaderSetupPacket(rn, APP_SHADER_SETUPS.axis_lines); //Axis lines setup
				UniformBindCamera(rn);
				{
					const hf::ShaderBindingInfo vikingRoomShaderInfo
					{
						.shader = APP_SHADERS.axis_lines,
						.attrib = hf::resources::GetQuadBufferAttrib(),
						.bindingPoint = hf::RenderBindingType::Graphics
					};
					hf::draw::StartShaderPacket(rn, vikingRoomShaderInfo);
					{
						hf::draw::StartMaterialPacket(rn, APP_MATERIALS.axis_lines);
						{
							axisLines.Draw(rn);
						}
						hf::draw::EndMaterialPacket(rn);
					}
					hf::draw::EndShaderPacket(rn);
				}
				hf::draw::EndShaderSetupPacket(rn);
			}
		}

		VoxelTerrainDraw(rn);

		hf::draw::EndRenderPassPacket(rn);
	}
}
