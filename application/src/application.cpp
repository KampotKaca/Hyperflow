#include "application.h"
#include "debug.h"
#include "resources.h"

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
	}

	void AppUpdate()
	{
		vikingRoomTransform.euler.y -= (float)hf::time::GetDeltaTime() * 10.0f;
		freeMoveCamera.Update(hf::GetMainWindow(), (float)hf::time::GetDeltaTime());
		DebugUpdate();

		if (hf::input::IsDown(hf::Key::N)) drawAxisLines = !drawAxisLines;
	}

	void AppQuit()
	{
		DebugQuit();
	}

	void AppPreRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugPreRender(rn);
	}

	void AppRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugRender(rn);

		hf::renderpass::Begin(rn, APP_RENDER_PASSES.mainPresentPass);
		hf::shadersetup::Bind(rn, APP_SHADER_SETUPS.viking_room);

		UniformUploadCameraTime(rn, freeMoveCamera.camera3D.core,
			freeMoveCamera.camera3D.direction, freeMoveCamera.camera3D.position,
			freeMoveCamera.camera3D.ToViewMat4(), vikingRoomTransform.ToMat4());
		// UniformUploadCameraTime(rn, anchoredCamera.core, anchoredCamera.ToViewMat4());

		hf::shader::Bind(rn, APP_SHADERS.viking_room, APP_BUFFER_ATTRIBUTES.pos_col_tex);
		hf::texturepack::Bind(rn, APP_TEXTURE_PACKS.viking_room_pack);

		hf::renderer::Draw(rn, APP_MESHES.viking_room);

		if (drawAxisLines)
		{
			hf::shadersetup::Bind(rn, APP_SHADER_SETUPS.axis_lines);
			UniformBindCameraTime(rn);

			hf::shader::Bind(rn, APP_SHADERS.axis_lines, hf::bufferattrib::GetQuad());

			for (uint32_t i = 0; i < 100; i++)
			{
				axisLines.Draw(rn);
			}
		}

		hf::renderpass::End(rn);
	}
}
