#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <cstdint>
#include "math.h"

using namespace Math;

enum Material_t : uint8_t
{
	M_VOID = 0,
	M_DIRT = 1,
	M_ROCK = 2,
	M_OBSIDIAN = 3,
	M_MOSS = 4,
	M_WATER = 20,
	M_LAVA = 21
};

enum Level_t : uint8_t
{
	L_EARTH = 0
};

struct Pixel
{
	uint8_t n;						// noise value
	int32_t x, y;					// xy-coords in bitmap
	Material_t m;					// material id
	int8_t r;						// color: red
	int8_t g;						// color: green
	int8_t b;						// color: blue
};

struct LevelConfig
{
	uint32_t seed;					// initial seed
	Level_t type;					// generator type
	int32_t width;					// bitmap width
	int32_t height;					// bitmap height
	float n_scale;					// noise scale
	uint8_t n_water;				// 0..255
	uint8_t n_lava;					// 0..255
};

class Level
{
public:
	Level(
		LevelConfig config
	);
	~Level();

	void generate();
	void generate_clumps(Material_t m, Material_t t, size_t amount, uint8_t chance, uint8_t n_min = 0, uint8_t n_max = 0);
	void regenerate(uint32_t seed);
	void sample_pixel(Pixel * pixel, int32_t offset_x = 0, int32_t offset_y = 0);
	void alter(Material_t m, uint8_t r, int32_t x, int32_t y);
	void render();
	void update();
private:
	LevelConfig m_cfg;
	int32_t m_width;
	int32_t m_height;
	SimplexGen m_simplex;
	std::vector<Pixel> m_bitmap;
	std::vector<Pixel *> m_liquid;
};

#endif // LEVEL_H