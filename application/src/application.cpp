#include "application.h"
#include "debug.h"
#include "resources.h"
#include "voxelterrain/voxelterrain.h"

namespace app
{
	AppObjects APP_OBJECTS;

	void AppRendererLoad()
	{
		VertexAttributesDefineAll();
		TextureSamplerDefineAll();
		TextureLayoutDefineAll();
		ShaderLayoutDefineAll();
		DebugDefine();
	}

	void AppLoadResources()
	{
		CubemapLoadAll();
	    CreateRenderTextures();

		TexturePackLoadAll();
		ShaderLoadAll();
		MaterialLoadAll();

	    {
		    const hf::AudioListenerCreationInfo info{};
		    APP_OBJECTS.mainListener = hf::Create(info);
	    }

		DebugLoad();
	}

	void AppStart()
	{
		DebugStart();

		VoxelTerrainGenerate();
		hf::SetTargetFrameRate(170);
	    SetWindowState(hf::WindowState::Maximized);
	    APP_OBJECTS.mainScene = hf::LoadScene<ApplicationScene>();
	}

    void AppPreUpdate()
	{
	    DebugUpdate();
	    VoxelTerrainUpdate();
	}

    void AppPostUpdate()
	{

	}

	void AppQuit()
	{
		VoxelTerrainDispose();
		DebugQuit();
	}
}
