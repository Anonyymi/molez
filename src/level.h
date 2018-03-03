#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <cstdint>
#include "math.h"

using namespace Math;

enum Material_t : uint8_t
{
	M_VOID = 0,
	M_SOLID = 1,
	M_SOLID_ID = 2,
	M_FLUID = 3
};

enum Texture_t : uint8_t
{
	T_NULL = 0,
	T_AIR = 1,
	T_DIRT = 2,
	T_ROCK = 3,
	T_MOSS = 4,
	T_OBSIDIAN = 5,
	T_WATER = 6,
	T_LAVA = 7
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
	Texture_t t;					// texture id
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
	void alter(Material_t m, Texture_t t, uint8_t r, int32_t x, int32_t y, bool edit = false);
	void draw(Material_t m, Texture_t t, int32_t x, int32_t y);
	void samplePixel(Pixel * p);
	std::string sampleTexture(Texture_t t);
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