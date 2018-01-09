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

class Level
{
public:
	Level(
		Level_t type = L_EARTH,
		size_t width = 640,
		size_t height = 467,
		uint32_t seed = 1234
	);
	~Level();

	void generate();
	void regenerate(uint32_t seed);
	void sample_pixel(Pixel * pixel);
	void alter(Material_t material, int32_t radius, int32_t x, int32_t y);
	void render();
	void update();
private:
	Level_t m_type;
	size_t m_width;
	size_t m_height;
	SimplexGen m_nGenSimplex;
	std::vector<Pixel> m_bitmap;
	std::vector<Pixel *> m_liquid;
};

#endif // LEVEL_H