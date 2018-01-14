#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <stdint.h>
#include <string>



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

#endif