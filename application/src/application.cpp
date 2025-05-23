#include "application.h"
#include <hyperflow.h>

#include "resources/renderpasses.h"
#include "resources/uniforms.h"
#include "resources/textures.h"
#include "resources/meshes.h"
#include "resources/shaders.h"
#include "resources/bufferattributes.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"
#include "resources/texturepacks.h"
#include "resources/shadersetups.h"
#include "debug.h"

namespace app
{
	hf::Camera3D camera{};

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
		hf::shadersetup::Bind(rn, APP_SHADER_SETUPS.viking_room_setup);

		UniformUploadCameraTime(rn, camera);

		hf::shader::Bind(rn, APP_SHADERS.viking_room_shader, APP_BUFFER_ATTRIBUTES.pctAttribute);
		hf::texturepack::Bind(rn, APP_TEXTURE_PACKS.viking_room_pack);

		hf::renderer::Draw(rn, APP_MESHES.viking_room);

		hf::renderpass::End(rn);
	}
}
