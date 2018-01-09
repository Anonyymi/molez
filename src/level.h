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
	M_OBSIDIAN = 2,
	M_WATER = 20,
	M_LAVA = 21,
	M_SLIME = 22
};

enum Level_t : uint8_t
{
	L_EARTH = 0
};

struct Pixel
{
	int32_t x, y;
	Material_t m;
	int8_t r;
	int8_t g;
	int8_t b;
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
	void regenerate(uint32_t seed);
	void sample_pixel(Pixel * pixel);
	void alter(Material_t m, int32_t r, int32_t x, int32_t y);
	void render();
	void update();
private:
	LevelConfig m_cfg;
	SimplexGen m_simplex;
	std::vector<Pixel> m_bitmap;
	std::vector<Pixel *> m_liquid;
};

#endif // LEVEL_H