#ifndef VOXELTERRAINCONFIG_H
#define VOXELTERRAINCONFIG_H

//region Caves
#define VOXEL_TERRAIN_CAVE_NOISE FNL_NOISE_PERLIN
#define VOXEL_TERRAIN_CAVE_FRACTAL FNL_FRACTAL_PINGPONG
#define VOXEL_TERRAIN_CAVE_FREQUENCY .01f
#define VOXEL_TERRAIN_CAVE_OCTAVES 4
#define VOXEL_TERRAIN_CAVE_GAIN .5f
#define VOXEL_TERRAIN_CAVE_LACUNARITY 2
#define VOXEL_TERRAIN_CAVE_PING_PONG_STRENGTH 2
//endregion

//region Flats
#define VOXEL_TERRAIN_FLAT_NOISE FNL_NOISE_VALUE_CUBIC
#define VOXEL_TERRAIN_FLAT_FRACTAL FNL_FRACTAL_FBM
#define VOXEL_TERRAIN_FLAT_FREQUENCY .01f
#define VOXEL_TERRAIN_FLAT_OCTAVES 6
#define VOXEL_TERRAIN_FLAT_GAIN .5f
#define VOXEL_TERRAIN_FLAT_LACUNARITY 2
//endregion

//region Small Hills
#define VOXEL_TERRAIN_SMALL_HILL_NOISE FNL_NOISE_VALUE
#define VOXEL_TERRAIN_SMALL_HILL_FRACTAL FNL_FRACTAL_FBM
#define VOXEL_TERRAIN_SMALL_HILL_FREQUENCY .01f
#define VOXEL_TERRAIN_SMALL_HILL_OCTAVES 6
#define VOXEL_TERRAIN_SMALL_HILL_GAIN .5f
#define VOXEL_TERRAIN_SMALL_HILL_LACUNARITY 2
//endregion

//region Hills
#define VOXEL_TERRAIN_HILL_NOISE FNL_NOISE_PERLIN
#define VOXEL_TERRAIN_HILL_FRACTAL FNL_FRACTAL_FBM
#define VOXEL_TERRAIN_HILL_FREQUENCY .01f
#define VOXEL_TERRAIN_HILL_OCTAVES 6
#define VOXEL_TERRAIN_HILL_GAIN .5f
#define VOXEL_TERRAIN_HILL_LACUNARITY 2
//endregion

//region Mountains
#define VOXEL_TERRAIN_MOUNTAIN_NOISE FNL_NOISE_OPENSIMPLEX2S
#define VOXEL_TERRAIN_MOUNTAIN_FRACTAL FNL_FRACTAL_FBM
#define VOXEL_TERRAIN_MOUNTAIN_FREQUENCY .01f
#define VOXEL_TERRAIN_MOUNTAIN_OCTAVES 6
#define VOXEL_TERRAIN_MOUNTAIN_GAIN .5f
#define VOXEL_TERRAIN_MOUNTAIN_LACUNARITY 2
//endregion

//region High Mountains
#define TERRAIN_HIGH_MOUNTAIN_NOISE FNL_NOISE_OPENSIMPLEX2
#define TERRAIN_HIGH_MOUNTAIN_FRACTAL FNL_FRACTAL_FBM
#define TERRAIN_HIGH_MOUNTAIN_FREQUENCY .01f
#define TERRAIN_HIGH_MOUNTAIN_OCTAVES 6
#define TERRAIN_HIGH_MOUNTAIN_GAIN .5f
#define TERRAIN_HIGH_MOUNTAIN_LACUNARITY 2
//endregion

#endif //VOXELTERRAINCONFIG_H
