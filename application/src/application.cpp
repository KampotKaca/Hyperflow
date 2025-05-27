#include "application.h"
#include "debug.h"
#include "resources.h"

namespace app
{
	hf::FreeMoveCamera3D freeMoveCamera{};
	hf::Camera3DAnchored anchoredCamera{};
	hf::Transform viking_room_transform{};

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
		freeMoveCamera.camera3D.position = {0, 0, -2};
		freeMoveCamera.camera3D.direction = {0, 0, 1};
		// freeMoveCamera.camera3D.distance = 2;
		UniformStartAll();
		DebugStart();
	}

	void AppUpdate()
	{
		viking_room_transform.euler.y -= (float)hf::time::GetDeltaTime() * 10.0f;
		freeMoveCamera.Update(hf::GetMainWindow(), (float)hf::time::GetDeltaTime());
		DebugUpdate();
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
		hf::shadersetup::Bind(rn, APP_SHADER_SETUPS.viking_room_setup);

		UniformUploadCameraTime(rn, freeMoveCamera.camera3D.core, freeMoveCamera.camera3D.ToViewMat4(), viking_room_transform.ToMat4());
		// UniformUploadCameraTime(rn, anchoredCamera.core, anchoredCamera.ToViewMat4());

		hf::shader::Bind(rn, APP_SHADERS.viking_room_shader, APP_BUFFER_ATTRIBUTES.pctAttribute);
		hf::texturepack::Bind(rn, APP_TEXTURE_PACKS.viking_room_pack);

		hf::renderer::Draw(rn, APP_MESHES.viking_room);

		hf::renderpass::End(rn);
	}
}
