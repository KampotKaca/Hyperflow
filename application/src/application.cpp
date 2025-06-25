#include "application.h"
#include "debug.h"
#include "resources.h"
#include "voxelterrain.h"

namespace app
{
	AppObjects APP_OBJECTS;

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
		APP_OBJECTS.vikingRoom2Transform.position = { 3, 0, 3 };
		UniformStartAll();
		DebugStart();

		// VoxelTerrainGenerate();
		hf::SetTargetFrameRate(240);
	}

	void AppUpdate()
	{
		APP_OBJECTS.vikingRoomTransform.euler.y -= (float)hf::GetDeltaTime() * 10.0f;
		APP_OBJECTS.vikingRoom2Transform.euler.y -= (float)hf::GetDeltaTime() * 10.0f;
		DebugUpdate();
		// VoxelTerrainUpdate();
	}

	void AppQuit()
	{
		// VoxelTerrainDispose();
		DebugQuit();
	}
}
