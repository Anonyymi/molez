#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <cstdint>
#include "math.h"
#include "Material.hpp"
#include <stdlib.h>

using namespace Math;



enum Level_t : uint8_t
{
	L_EARTH = 0
};

struct Pixel{
	/*
	uint8_t n;						// noise value
	int32_t x, y;					// xy-coords in bitmap
	Material_t m;					// material id
	*/
	union{
		uint32_t rgba;
		struct{
			uint8_t r;						// color: red
			uint8_t g;						// color: green
			uint8_t b;						// color: blue
			uint8_t a;
		};
	};
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

typedef struct Rectangle{
	uint32_t x,y,x1,y1;
} Rectangle;

typedef enum Direction : uint8_t{
	DOWN = 0,
	DOWNLEFT = 1,
	DOWNRIGHT = 2,
	RIGHT = 3,
	LEFT = 4,
	STAY
} Direction;

typedef struct Area{
	//
	Rectangle dimensions;
	Material **materialmap;
	Pixel **bitmap;
	Direction **gravitymap;
	Vector2 **vectormap;
} Area;


class Level
{
public:


	Level();
	~Level();

	void freeAreaBuffers(Area &area);
	void allocateAreaBuffers(Area &area);

	void setConfig(LevelConfig cfg);
	void generate();
	void updateGravity(uint32_t y,uint32_t x, Area &area);
	void updateGravity(uint32_t y,uint32_t x);
	void generateGravitymap(uint32_t height);
	void generate_clumps(Material_t m, Material_t t, size_t amount, uint8_t chance, uint8_t n_min = 0, uint8_t n_max = 0);
	void regenerate(uint32_t seed);
	uint32_t sample_pixel(uint32_t s_y, uint32_t s_x, Area &area);
	void swapLocation(int srcY, int srcX, int destY, int destX, Area &area);
	void alter(Material_t m, uint8_t r, int32_t x, int32_t y);
	void swapPixel(uint32_t y0,uint32_t x0,uint32_t y1,uint32_t x1);
	void render(uint32_t height);
	void update(uint32_t y,uint32_t x);

private:
	LevelConfig levelCfg;
	SimplexGen m_simplex;
	std::vector<Area> areas;
};




#endif // LEVEL_H