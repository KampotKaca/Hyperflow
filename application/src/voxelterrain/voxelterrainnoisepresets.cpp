#include "voxelterrain/voxelterraingeneration.h"
#include "voxelterrain/voxelterrainconfig.h"

namespace app
{
    fnl_state VoxelTerrainGetCaveNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type		 = VOXEL_TERRAIN_CAVE_NOISE;
		noise.fractal_type		 = VOXEL_TERRAIN_CAVE_FRACTAL;
		noise.frequency			 = VOXEL_TERRAIN_CAVE_FREQUENCY;
		noise.octaves			 = VOXEL_TERRAIN_CAVE_OCTAVES;
		noise.gain				 = VOXEL_TERRAIN_CAVE_GAIN;
		noise.lacunarity		 = VOXEL_TERRAIN_CAVE_LACUNARITY;
		noise.ping_pong_strength = VOXEL_TERRAIN_CAVE_PING_PONG_STRENGTH;
		return noise;
	}

	fnl_state VoxelTerrainGetFlatNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type   = VOXEL_TERRAIN_FLAT_NOISE;
		noise.fractal_type = VOXEL_TERRAIN_FLAT_FRACTAL;
		noise.frequency	   = VOXEL_TERRAIN_FLAT_FREQUENCY;
		noise.octaves	   = VOXEL_TERRAIN_FLAT_OCTAVES;
		noise.gain		   = VOXEL_TERRAIN_FLAT_GAIN;
		noise.lacunarity   = VOXEL_TERRAIN_FLAT_LACUNARITY;
		return noise;
	}

	fnl_state VoxelTerrainGetSmallHillNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type   = VOXEL_TERRAIN_FLAT_NOISE;
		noise.fractal_type = VOXEL_TERRAIN_FLAT_FRACTAL;
		noise.frequency	   = VOXEL_TERRAIN_FLAT_FREQUENCY;
		noise.octaves	   = VOXEL_TERRAIN_FLAT_OCTAVES;
		noise.gain		   = VOXEL_TERRAIN_FLAT_GAIN;
		noise.lacunarity   = VOXEL_TERRAIN_FLAT_LACUNARITY;
		return noise;
	}

	fnl_state VoxelTerrainGetHillNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type   = VOXEL_TERRAIN_SMALL_HILL_NOISE;
		noise.fractal_type = VOXEL_TERRAIN_SMALL_HILL_FRACTAL;
		noise.frequency	   = VOXEL_TERRAIN_SMALL_HILL_FREQUENCY;
		noise.octaves	   = VOXEL_TERRAIN_SMALL_HILL_OCTAVES;
		noise.gain		   = VOXEL_TERRAIN_SMALL_HILL_GAIN;
		noise.lacunarity   = VOXEL_TERRAIN_SMALL_HILL_LACUNARITY;
		return noise;
	}

	fnl_state VoxelTerrainGetMountainNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type   = VOXEL_TERRAIN_MOUNTAIN_NOISE;
		noise.fractal_type = VOXEL_TERRAIN_MOUNTAIN_FRACTAL;
		noise.frequency    = VOXEL_TERRAIN_MOUNTAIN_FREQUENCY;
		noise.octaves      = VOXEL_TERRAIN_MOUNTAIN_OCTAVES;
		noise.gain         = VOXEL_TERRAIN_MOUNTAIN_GAIN;
		noise.lacunarity   = VOXEL_TERRAIN_MOUNTAIN_LACUNARITY;
		return noise;
	}

	fnl_state VoxelTerrainGetHighMountainNoise()
	{
		fnl_state noise = fnlCreateState();
		noise.noise_type   = TERRAIN_HIGH_MOUNTAIN_NOISE;
		noise.fractal_type = TERRAIN_HIGH_MOUNTAIN_FRACTAL;
		noise.frequency    = TERRAIN_HIGH_MOUNTAIN_FREQUENCY;
		noise.octaves      = TERRAIN_HIGH_MOUNTAIN_OCTAVES;
		noise.gain         = TERRAIN_HIGH_MOUNTAIN_GAIN;
		noise.lacunarity   = TERRAIN_HIGH_MOUNTAIN_LACUNARITY;
		return noise;
	}

	VoxelTerrainNoiseSetup VoxelTerrainGetNoiseSetup()
    {
	    return VoxelTerrainNoiseSetup
    	{
    		.caveNoise = VoxelTerrainGetCaveNoise(),
    		.flatNoise = VoxelTerrainGetFlatNoise(),
    		.smallHillNoise = VoxelTerrainGetSmallHillNoise(),
    		.hillNoise = VoxelTerrainGetHillNoise(),
    		.mountainNoise = VoxelTerrainGetMountainNoise(),
    		.highMountainNoise = VoxelTerrainGetHighMountainNoise()
    	};
    }
}