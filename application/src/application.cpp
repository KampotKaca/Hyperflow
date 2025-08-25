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
		APP_OBJECTS.sphere.transform.position = hf::vec3{ -3, 1, 3 };

	    for (uint32_t x = 0; x < VIKING_ROOM_AXIS_SIZE; x++)
	        for (uint32_t z = 0; z < VIKING_ROOM_AXIS_SIZE; z++)
	            APP_OBJECTS.vikingRooms[x * VIKING_ROOM_AXIS_SIZE + z].transform.position = hf::vec3{ x, 0, z } * 2.0f - hf::vec3{ VIKING_ROOM_AXIS_SIZE, 0, VIKING_ROOM_AXIS_SIZE };

		DebugStart();

		VoxelTerrainGenerate();
		hf::SetTargetFrameRate(170);
	    SetState(hf::GetMainWindow(), hf::WindowState::Maximized);
	}

	void AppUpdate()
	{
		APP_OBJECTS.ground.transform.scale = hf::vec3{ 100, 1, 100 };
		DebugUpdate();
		VoxelTerrainUpdate();
	}

	void AppQuit()
	{
		VoxelTerrainDispose();
		DebugQuit();
	}
}
