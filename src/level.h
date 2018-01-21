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
	M_MOSS = 3,
	M_OBSIDIAN = 4,
	M_WATER = 20,
	M_LAVA = 21
};

enum Level_t : uint8_t
{
	L_EARTH = 0
};

struct Pixel
{
	int32_t x, y;					// xy-coords in bitmap
	uint8_t n;						// noise value
	Material_t m;					// material id
	int32_t argb;					// color, ARGB
};

struct LevelConfig
{
	uint32_t seed;					// initial seed
	Level_t type;					// generator type
	int32_t width;					// bitmap width
	int32_t height;					// bitmap height
	float n_scale;					// noise scale
	uint8_t dirt_n;					// 0..255
	uint8_t object_n;				// 0..255
	uint8_t water_n;				// 0..255
	uint8_t lava_n;					// 0..255
};

class Level
{
public:
	Level(
		LevelConfig config
	);
	~Level();

	void gen();
	void genObject();
	void genFluid();
	void regen(uint32_t seed);
	void alter(Material_t m, uint8_t r, int32_t x, int32_t y, bool edit = false);
	void draw(Material_t m, int32_t x, int32_t y);
	void samplePixel(Pixel * p);
	std::string sampleMaterial(Material_t m);
	void update(float state, float t, float dt);
	void render(float state);

	void setCfg(LevelConfig cfg);
	LevelConfig & getCfg();
private:
	LevelConfig m_cfg;
	int32_t m_width;
	int32_t m_height;
	SimplexGen m_simplex;
	std::vector<Pixel> m_bitmap;
	std::vector<Pixel *> m_fluid;
};

#endif // LEVEL_H