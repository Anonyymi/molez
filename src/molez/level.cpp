#include "level.h"
#include <random>
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "math.h"



Level::Level(){
	Area a;
	areas.push_back(a);

}

Level::~Level(){
	for(auto &area : areas)
		freeAreaBuffers(area);

}

void Level::freeAreaBuffers(Area &area){
	/*
	for(auto& area : areas){
		for(uint32_t i = 0; i<levelCfg.height;i++){
			if(area.materialmap[i] != NULL)
				free(area.materialmap[i]);
			if(area.bitmap[i] != NULL)
				free(area.bitmap[i]);
		}
		if(area.materialmap != NULL)
			free(area.materialmap);
		if(area.bitmap != NULL)
			free(area.bitmap);
	}
	*/
}
void Level::allocateAreaBuffers(Area &area){
	area.materialmap = (Material**)malloc(sizeof(Material*)*levelCfg.height);
	area.bitmap = (Pixel**)malloc(sizeof(Pixel*)*levelCfg.height);
	area.gravitymap = (Direction**)malloc(sizeof(Direction*)*levelCfg.height);
	area.vectormap = (Vector2**)malloc(sizeof(Vector2*)*levelCfg.height);
	for(uint32_t i = 0; i<levelCfg.height;i++){
		area.materialmap[i] = (Material*)malloc(sizeof(Material)*levelCfg.width);
		area.bitmap[i] = (Pixel*)malloc(sizeof(Pixel)*levelCfg.width);
		area.gravitymap[i] = (Direction*)malloc(sizeof(Direction)*levelCfg.width);
		area.vectormap[i] = (Vector2*)malloc(sizeof(Vector2)*levelCfg.width);
	}
}



void Level::setConfig(LevelConfig cfg){
	levelCfg = cfg;
	m_simplex.reseed(levelCfg.seed);

}

void Level::generate(){

	
	// Reset liquids
	// Iterate through each pixel to generate level geometry
	for(auto &area : areas){
		freeAreaBuffers(area);
		allocateAreaBuffers(area);
		for (int32_t i = 0; i < levelCfg.height; i++){
			for (int32_t j = 0; j < levelCfg.width; j++){
				// Noise value at x,y, re-scaled from -1,+1 to 0,+1
				float n_value = m_simplex.noise(j * levelCfg.n_scale, i * levelCfg.n_scale);
				n_value += 1.0f;
				n_value *= 0.5f;

				// Noise value at x,y, re-scaled from 0,+1 to 0,255
				int n_ivalue = static_cast<int>(n_value * 255.0f);

				// Select material
				if (n_value <= 0.33f){
					area.materialmap[i][j].id = M_VOID;
					area.materialmap[i][j].state = GAS;
				}
				else if (n_value >= 0.33f){
					area.materialmap[i][j].id = M_DIRT;
					area.materialmap[i][j].state = SOLID;
				}
				// Sample texture rgb for the pixel
				area.bitmap[i][j].rgba = sample_pixel(i, j, area);
			}
		}

	}

	// Generate solids
	//generate_clumps(M_ROCK, M_DIRT, 128, RNG_DIST32(RNG) % 191, 128, 255);

	// Generate liquids
	//generate_clumps(M_WATER, M_VOID, 255, levelCfg.n_water);
	//generate_clumps(M_LAVA, M_VOID, 255, levelCfg.n_lava);

	mlibc_inf("Level::generate(). Level generated! Type: %u, width: %zu, height: %zu", levelCfg.type, levelCfg.width, levelCfg.height);
}

void Level::updateGravity(uint32_t y, uint32_t x, Area &area){
	
	if(area.materialmap[y+1][x].id == M_VOID)
		area.gravitymap[y][x] = DOWN;
	else if(area.materialmap[y+1][x+1].id == M_VOID)
		area.gravitymap[y][x] = DOWNRIGHT;
	else if(area.materialmap[y+1][x-1].id == M_VOID)
		area.gravitymap[y][x] = DOWNLEFT;
	else if(area.materialmap[y][x+1].id == M_VOID)
		area.gravitymap[y][x] = RIGHT;
	else if(area.materialmap[y][x-1].id == M_VOID)
		area.gravitymap[y][x] = LEFT;
	else
		area.gravitymap[y][x] = STAY;
}
void Level::updateGravity(uint32_t y, uint32_t x){
	updateGravity(y,x,areas.at(0));
}

void Level::generateGravitymap(uint32_t height){
	Area area = areas.at(0);
	for(int i=1;i<levelCfg.width-1;i++){
		updateGravity(height,i,area);
	}

}

void Level::generate_clumps(Material_t m, Material_t t, size_t amount, uint8_t chance, uint8_t n_min, uint8_t n_max){
	/*
	for (size_t i = 0; i < amount; i++)
	{
		uint8_t rng_val = static_cast<uint8_t>(RNG_DIST8(RNG));

		if (rng_val < chance)
		{
			// Water insert properties
			uint8_t r = static_cast<uint8_t>(RNG_DIST8(RNG)) % 16 + 2;
			int32_t x = RNG_DIST32(RNG) % levelCfg.width;
			int32_t y = RNG_DIST32(RNG) % levelCfg.height;

			// Get pixel
			Pixel * p = &m_bitmap[x + y * levelCfg.width];

			// Adjust radius with noise value
			r += (p->n / 96);

			// Only insert if current pixel is TARGET
			if (n_min == 0 && n_max == 0)
			{
				if (p->m != t)
					continue;
			}
			// Only insert between chosen noise values
			else
			{
				if (p->n < n_min || p->n > n_max)
					continue;
			}

			// Alter the level
			alter(m, r, x, y);
		}
	}
	*/
}

void Level::regenerate(uint32_t seed){
	mlibc_inf("Level::regenerate(%u). Regenerating level...", seed);

	// Re-seed noise generator(s)
	m_simplex.reseed(seed);

	// Re-generate the level
	generate();
}

uint32_t Level::sample_pixel(uint32_t s_y, uint32_t s_x, Area &area){
	// Final int32_t HEX ARGB color
	uint32_t argb = 0;

	// Determine texture/properties by material
	switch (area.materialmap[s_y][s_x].id)
	{
		case M_VOID:		argb = 0xFF000000; break;
		case M_DIRT:		argb = TextureManager::sample_texture("DIRT.PNG", s_x, s_y); break;
		case M_ROCK:		argb = TextureManager::sample_texture("ROCK1.PNG", s_x, s_y); break;
		case M_OBSIDIAN:	argb = TextureManager::sample_texture("OBSIDIAN.PNG", s_x, s_y); break;
		case M_MOSS:		argb = TextureManager::sample_texture("MOSS.PNG", s_x, s_y); break;
		case M_WATER:		argb = TextureManager::sample_texture("WATER.PNG", s_x, s_y); break;
		case M_LAVA:		argb = TextureManager::sample_texture("LAVA.PNG", s_x, s_y); break;
	}

	return argb;
}

void Level::
swapLocation(int srcY, int srcX, int destY, int destX, Area &area){
	Material tmp = area.materialmap[destY][destX];
	Pixel tmpp = area.bitmap[destY][destX];
	area.materialmap[destY][destX] = area.materialmap[srcY][srcX];
	area.bitmap[destY][destX] = area.bitmap[srcY][srcX];
	area.materialmap[srcY][srcX] = tmp;
	area.bitmap[srcY][srcX] = tmpp;
}

void Level::alter(Material_t m, uint8_t r, int32_t x, int32_t y){
	// Calculate start coords
	int32_t x_start = x - r;
	int32_t y_start = y - r;

	Area area = areas.at(0);

	// Iterate over the area we want to alter
	for (int32_t i = y_start; i < y_start + r * 2; i++)
	{
		for (int32_t j = x_start; j < x_start + r * 2; j++)
		{
			// Get distance from center to current pos
			int32_t d = static_cast<int32_t>(std::sqrt(((j - x) * (j - x)) + ((i - y) * (i - y))));

			// Alter only if we are within the given radius
			if (d < r)
			{
				// Do not allow altering memory outside level bounds
				if (j < 0 || j >= levelCfg.width || i < 0 || i >= levelCfg.height)
				{
					continue;
				}

				area.materialmap[i][j].id = m;
				area.materialmap[i][j].state = LIQUID;
				area.bitmap[i][j].rgba = sample_pixel(i,j,area);
			}
		}
	}
}

void Level::render(uint32_t height){

}


void Level::update(uint32_t y, uint32_t x){
	uint32_t tmp;
	Area area = areas.at(0);
	if(area.materialmap[y][x].state == LIQUID){
		updateGravity(y,x,area);
		switch(area.gravitymap[y][x]){
			case DOWN:
				swapLocation(y,x,y+1,x,area);				
				break;
			case DOWNLEFT:						
				swapLocation(y,x,y+1,x-1,area);
				break;
			case DOWNRIGHT:						
				swapLocation(y,x,y+1,x+1,area);
				break;
			case RIGHT:					
				swapLocation(y,x,y,x+1,area);
				break;
			case LEFT:					
				swapLocation(y,x,y+1,x,area);
			break;
		}
		
	}

}


Pixel **Level::getBitmap(){
	return areas.at(0).bitmap;
}

