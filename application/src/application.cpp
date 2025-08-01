#include "application.h"
#include "debug.h"
#include "resources.h"
#include "voxelterrain/voxelterrain.h"

namespace app
{
	AppObjects APP_OBJECTS;

	void AppRendererLoad()
	{
		TextureSamplerDefineAll();
		TextureLayoutDefineAll();
		BufferAttributeDefineAll();
		UniformDefineAll();
		ShaderLayoutDefineAll();
		DebugDefine();
	}

	void AppLoadResources()
	{
		TextureLoadAll();
		CubemapLoadAll();
	    CreateRenderTextures();

        hf::SubmitAllTextures();

		MeshLoadAll();
		TexturePackLoadAll();
		ShaderLoadAll();
		MaterialLoadAll();
		AudioLoadAll();
		DebugLoad();
	}

	void AppStart()
	{
		APP_OBJECTS.sphereTransform.position = { -3, 1, 3 };
		UniformStartAll();
		DebugStart();

		VoxelTerrainGenerate();
		hf::SetTargetFrameRate(170);
	    SetState(hf::GetMainWindow(), hf::WindowState::Maximized);
	}

	void AppUpdate()
	{
		APP_OBJECTS.vikingRoomTransform.euler.y -= (float)hf::GetDeltaTime() * 10.0f;
		APP_OBJECTS.groundTransform.scale = { 100, 1, 100 };
		DebugUpdate();
		VoxelTerrainUpdate();
	}

	void AppQuit()
	{
		VoxelTerrainDispose();
		DebugQuit();
	}
}
