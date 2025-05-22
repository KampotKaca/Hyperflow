#include "application.h"
#include <hyperflow.h>

#include "renderpasses.h"
#include "uniforms.h"
#include "textures.h"
#include "meshes.h"
#include "shaders.h"
#include "debug.h"

namespace app
{
	hf::Camera3D camera{};

	void AppRendererLoad()
	{
		TextureDefineAll();
		MeshDefineAll();
		UniformDefineAll();
		ShaderDefineAll();
		DebugDefine();
	}

	void AppLoadResources()
	{
		TextureLoadAll();
		MeshLoadAll();
		ShaderLoadAll();
		DebugLoad();
	}

	void AppStart()
	{
		UniformStartAll();
		DebugStart();

        camera.distance = 2;
	}

	void AppUpdate()
	{
		DebugUpdate();
	}

	void AppQuit()
	{
		DebugQuit();
	}

	void AppPreRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugPreRender();
	}

	void AppRender(const hf::Ref<hf::Renderer>& rn)
	{
		DebugRender();

		hf::renderpass::Begin(rn, APP_RENDER_PASSES.mainPresentPass);
		hf::shadersetup::Bind(rn, APP_SHADERS.viking_room_setup);

		UniformUploadCameraTime(rn, camera);

		hf::shader::Bind(rn, APP_SHADERS.viking_room_shader, APP_MESHES.meshAttribute);
		hf::texturepack::Bind(rn, APP_TEXTURES.viking_room_pack);

		hf::renderer::Draw(rn, APP_MESHES.viking_room);

		hf::renderpass::End(rn);
	}
}
