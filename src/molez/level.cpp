#include "level.h"
#include <random>
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "math.h"



Level::Level(){

}

Level::~Level(){
	freeBuffers();

}

void Level::freeBuffers(){
	if(materialmap == NULL || bitmap == NULL)
		return;
	for(uint32_t i = 0; i<levelCfg.height;i++){
		free(materialmap[i]);
		free(bitmap[i]);
	}
	free(materialmap);
	free(bitmap);
}
void Level::allocateBuffers(){
	materialmap = (Material_t**)malloc(sizeof(Material_t*)*levelCfg.height);
	bitmap = (Pixel**)malloc(sizeof(Pixel*)*levelCfg.height);
	gravitymap = (Direction**)malloc(sizeof(Direction*)*levelCfg.height);
	for(uint32_t i = 0; i<levelCfg.height;i++){
		materialmap[i] = (Material_t*)malloc(sizeof(Material_t)*levelCfg.width);
		bitmap[i] = (Pixel*)malloc(sizeof(Pixel)*levelCfg.width);
		gravitymap[i] = (Direction*)malloc(sizeof(Direction)*levelCfg.width);
	}
}



void Level::setConfig(LevelConfig cfg){
	levelCfg = cfg;
	m_simplex.reseed(levelCfg.seed);

}

void Level::generate(){

	freeBuffers();
	allocateBuffers();
	// Reset liquids
	// Iterate through each pixel to generate level geometry
	for (int32_t i = 0; i < levelCfg.height; i++)
	{
		for (int32_t j = 0; j < levelCfg.width; j++)
		{
			

			// Noise value at x,y, re-scaled from -1,+1 to 0,+1
			float n_value = m_simplex.noise(j * levelCfg.n_scale, i * levelCfg.n_scale);
			n_value += 1.0f;
			n_value *= 0.5f;

			// Noise value at x,y, re-scaled from 0,+1 to 0,255
			int n_ivalue = static_cast<int>(n_value * 255.0f);

			// Select material
			if (n_value <= 0.33f)
			{
				materialmap[i][j] = M_VOID;
			}
			else if (n_value >= 0.33f)
			{
				materialmap[i][j] = M_DIRT;
			}

			// Sample texture rgb for the pixel
			bitmap[i][j].rgba = sample_pixel(i, j);
		}
	}

	for(uint32_t i=1;i<levelCfg.height-1;i++)
		generateGravitymap(i);

	// Generate solids
	//generate_clumps(M_ROCK, M_DIRT, 128, RNG_DIST32(RNG) % 191, 128, 255);

	// Generate liquids
	//generate_clumps(M_WATER, M_VOID, 255, levelCfg.n_water);
	//generate_clumps(M_LAVA, M_VOID, 255, levelCfg.n_lava);

	mlibc_inf("Level::generate(). Level generated! Type: %u, width: %zu, height: %zu", levelCfg.type, levelCfg.width, levelCfg.height);
}

void Level::updateGravity(uint32_t y, uint32_t x){
	if(materialmap[y+1][x] == M_VOID)
		gravitymap[y][x] = DOWN;
	else if(materialmap[y+1][x+1] == M_VOID)
		gravitymap[y][x] = DOWNRIGHT;
	else if(materialmap[y+1][x-1] == M_VOID)
		gravitymap[y][x] = DOWNLEFT;
	else if(materialmap[y][x+1] == M_VOID)
		gravitymap[y][x] = RIGHT;
	else if(materialmap[y][x-1] == M_VOID)
		gravitymap[y][x] = LEFT;
	else
		gravitymap[y][x] = STAY;
}

void Level::generateGravitymap(uint32_t height){
	
	for(uint32_t i = 1; i<levelCfg.width-1; i++){
		updateGravity(height, i);

	}
	


}

void Level::generate_clumps(Material_t m, Material_t t, size_t amount, uint8_t chance, uint8_t n_min, uint8_t n_max)
{
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

void Level::regenerate(uint32_t seed)
{
	mlibc_inf("Level::regenerate(%u). Regenerating level...", seed);

	// Re-seed noise generator(s)
	m_simplex.reseed(seed);

	// Re-generate the level
	generate();
}

uint32_t Level::sample_pixel(uint32_t s_y, uint32_t s_x){
	// Final int32_t HEX ARGB color
	uint32_t argb = 0;

	// Determine texture/properties by material
	switch (materialmap[s_y][s_x]){
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

void Level::alter(Material_t m, uint8_t r, int32_t x, int32_t y)
{
	// Calculate start coords
	int32_t x_start = x - r;
	int32_t y_start = y - r;

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

				materialmap[i][j] = M_WATER;
				bitmap[i][j].rgba = sample_pixel(i,j);
			}
		}
	}
}

void Level::render(uint32_t height){
	for (uint32_t i = 0; i < levelCfg.width; i++){
		Pixel pixel = bitmap[height][i];
		//pixel.rgba = sample_pixel(height,i);
		/*
		pixel.r = gravitymap[height][i] * 2;
		pixel.g = gravitymap[height][i] * 3;
		pixel.b = gravitymap[height][i] * 2;
		*/
		// Set pixel to window fbo at x,y
		DisplayManager::set_pixel(i, height, pixel.b, pixel.g, pixel.r);
	}
}

void Level::swapPixel(uint32_t y0,uint32_t x0,uint32_t y1,uint32_t x1){
	uint32_t rgbatmp;
	rgbatmp = bitmap[y0][x0].rgba;
	bitmap[y0][x0].rgba = bitmap[y1][x1].rgba;
	bitmap[y1][y1].rgba = rgbatmp;
}

void Level::update(uint32_t height){

	for(uint32_t i = 1; i < levelCfg.width;i++){
		if(materialmap[height][i] == M_WATER){
			updateGravity(height,i);
			switch(gravitymap[height][i]){
				case DOWN:
					if(materialmap[height+1][i] == M_VOID){						
						materialmap[height+1][i] = M_WATER;
						materialmap[height][i] = M_VOID;
						bitmap[height][i].rgba = sample_pixel(height,i);
						bitmap[height+1][i].rgba = sample_pixel(height+1,i);
					}
					break;
				case DOWNLEFT:
					if(materialmap[height+1][i-1] == M_VOID){						
						materialmap[height+1][i-1] = M_WATER;
						materialmap[height][i] = M_VOID;
						bitmap[height][i].rgba = sample_pixel(height,i);
						bitmap[height+1][i-1].rgba = sample_pixel(height+1,i-1);
					}
					break;
				case DOWNRIGHT:
					if(materialmap[height+1][i+1] == M_VOID){						
						materialmap[height+1][i+1] = M_WATER;
						materialmap[height][i] = M_VOID;
						bitmap[height][i].rgba = sample_pixel(height,i);
						bitmap[height+1][i+1].rgba = sample_pixel(height+1,i+1);
					}
					break;
				case RIGHT:
					if(materialmap[height][i+1] == M_VOID){						
						materialmap[height][i+1] = M_WATER;
						materialmap[height][i] = M_VOID;
						bitmap[height][i].rgba = sample_pixel(height,i);
						bitmap[height][i+1].rgba = sample_pixel(height,i+1);
					}
					break;
				case LEFT:
					if(materialmap[height][i-1] == M_VOID){						
						materialmap[height][i-1] = M_WATER;
						materialmap[height][i] = M_VOID;
						bitmap[height][i].rgba = sample_pixel(height,i);
						bitmap[height][i-1].rgba = sample_pixel(height,i-1);
					}
					break;
			}
			
		}

	}


}