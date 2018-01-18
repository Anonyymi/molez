#include "level.h"
#include <random>
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "math.h"

Level::Level(
	LevelConfig config
) :
	m_cfg(config),
	m_width(m_cfg.width),
	m_height(m_cfg.height),
	m_simplex(m_cfg.seed),
	m_bitmap(m_width * m_height),
	m_liquid()
{

}

Level::~Level()
{

}

void Level::generate()
{
	// Reset dimensions (cfg dimensions can be altered)
	m_width = m_cfg.width;
	m_height = m_cfg.height;

	// Reset + resize bitmap
	m_bitmap.clear();
	m_bitmap.resize(m_width * m_height);

	// Reset liquids
	m_liquid.clear();

	// Iterate through each pixel to generate level geometry
	for (int32_t i = 0; i < m_height; i++)
	{
		for (int32_t j = 0; j < m_width; j++)
		{
			// Get pixel at x,y
			auto * pixel = &m_bitmap[j + i * m_width];

			// Init pixel value to M_VOID
			pixel->x = j;
			pixel->y = i;
			pixel->r = 0;
			pixel->g = 0;
			pixel->b = 0;
			pixel->m = M_VOID;

			// Noise value at x,y, re-scaled from -1,+1 to 0,+1
			float n_value = m_simplex.noise(j * m_cfg.n_scale, i * m_cfg.n_scale);
			n_value += 1.0f;
			n_value *= 0.5f;

			// Noise value at x,y, re-scaled from 0,+1 to 0,255
			int n_ivalue = static_cast<int>(n_value * 255.0f);

			pixel->n = static_cast<uint8_t>(n_ivalue);

			// Select material
			if (n_value <= 0.33f)
			{
				pixel->m = M_VOID;
			}
			else if (n_value >= 0.33f)
			{
				pixel->m = M_DIRT;
			}

			// Sample texture rgb for the pixel
			sample_pixel(pixel);
		}
	}

	// Generate solids
	generate_clumps(M_ROCK, M_DIRT, 128, RNG_DIST32(RNG) % 191, 128, 255);

	// Generate liquids
	generate_clumps(M_WATER, M_VOID, 255, m_cfg.n_water);
	generate_clumps(M_LAVA, M_VOID, 255, m_cfg.n_lava);

	mlibc_inf("Level::generate(). Level generated! Type: %u, width: %zu, height: %zu", m_cfg.type, m_width, m_height);
}

void Level::generate_clumps(Material_t m, Material_t t, size_t amount, uint8_t chance, uint8_t n_min, uint8_t n_max)
{
	for (size_t i = 0; i < amount; i++)
	{
		uint8_t rng_val = static_cast<uint8_t>(RNG_DIST8(RNG));

		if (rng_val < chance)
		{
			// Water insert properties
			uint8_t r = static_cast<uint8_t>(RNG_DIST8(RNG)) % 16 + 2;
			int32_t x = RNG_DIST32(RNG) % m_width;
			int32_t y = RNG_DIST32(RNG) % m_height;

			// Get pixel
			Pixel * p = &m_bitmap[x + y * m_width];

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
}

void Level::regenerate(uint32_t seed)
{
	mlibc_inf("Level::regenerate(%u). Regenerating level...", seed);

	// Re-seed noise generator(s)
	m_simplex.reseed(seed);

	// Re-generate the level
	generate();
}

void Level::sample_pixel(Pixel * pixel, int32_t offset_x, int32_t offset_y)
{
	// Final int32_t HEX ARGB color
	int32_t argb = 0;

	// Get sample xy-coords
	int32_t s_x = pixel->x - offset_x;
	int32_t s_y = pixel->y - offset_y;

	// Determine texture/properties by material
	switch (pixel->m)
	{
		case M_VOID:		argb = TextureManager::sample_texture("VOID.PNG", s_x, s_y); break;
		case M_DIRT:		argb = TextureManager::sample_texture("DIRT.PNG", s_x, s_y); break;
		case M_ROCK:		argb = TextureManager::sample_texture("ROCK1.PNG", s_x, s_y); break;
		case M_OBSIDIAN:	argb = TextureManager::sample_texture("OBSIDIAN.PNG", s_x, s_y); break;
		case M_MOSS:		argb = TextureManager::sample_texture("MOSS.PNG", s_x, s_y); break;
		case M_WATER:
		{
			argb = TextureManager::sample_texture("WATER.PNG", s_x, pixel->y);

			// This is a liquid
			m_liquid.push_back(pixel);
		} break;
		case M_LAVA:
		{
			argb = TextureManager::sample_texture("LAVA.PNG", s_x, pixel->y);

			// This is a liquid
			m_liquid.push_back(pixel);
		} break;
	}

	// Alpha 0x00 is transparency
	uint8_t a = (argb & 0xFF000000) >> 24;
	if (a == 0x00)
	{
		return;
	}

	// Assign new RGB values
	pixel->r = (argb & 0x00FF0000) >> 16;
	pixel->g = (argb & 0x0000FF00) >> 8;
	pixel->b = (argb & 0x000000FF);
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
				if (j < 0 || j >= m_width || i < 0 || i >= m_height)
				{
					continue;
				}

				// Get the pixel
				Pixel * pixel = &m_bitmap[j + i * m_width];

				// Alter it accordingly + resample
				pixel->m = m;
				sample_pixel(pixel);
			}
		}
	}
}

void Level::render()
{
	for (size_t i = 0; i < m_bitmap.size(); i++)
	{
		// Get pixel at x,y
		auto & pixel = m_bitmap[i];

		// Set pixel to window fbo at x,y
		DisplayManager::set_pixel(pixel.x, pixel.y, pixel.r, pixel.g, pixel.b);
	}
}

void Level::update()
{
	// Liquid physics
	size_t liquid_size = m_liquid.size();
	for (size_t i = 0; i < liquid_size; i++)
	{
		// Get liquid pixel at x,y
		Pixel * p_l = m_liquid[i];

		// If pixel is not a liquid material anymore, erase + skip it
		if (p_l->m == M_VOID)
		{
			// Erase
			m_liquid.erase(m_liquid.begin() + i);

			// Re-align loop size and iterator value
			liquid_size--;
			i--;

			continue;
		}

		// Neighbor pixel in our bitmap IF IT IS M_VOID
		Pixel * p_n = nullptr;

		// Pixel below us IF IT IS NOT M_VOID
		Pixel * p_n2 = nullptr;

		// Check pixel below
		if ((p_l->y + 1) < m_height)
		{
			p_n = &m_bitmap[p_l->x + (p_l->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+left
		if (p_n == nullptr && (p_l->y + 1) < m_height && (p_l->x - 1) < m_width)
		{
			p_n = &m_bitmap[(p_l->x - 1) + (p_l->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+right
		if (p_n == nullptr && (p_l->y + 1) < m_height && (p_l->x + 1) < m_width)
		{
			p_n = &m_bitmap[(p_l->x + 1) + (p_l->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel left
		if (p_n == nullptr && (p_l->x - 1) < m_width)
		{
			p_n = &m_bitmap[(p_l->x - 1) + p_l->y * m_width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel right
		if (p_n == nullptr && (p_l->x + 1) < m_width)
		{
			p_n = &m_bitmap[(p_l->x + 1) + p_l->y * m_width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Run liquid update
		if (p_n != nullptr)
		{
			// Copy values to new liquid pixel
			p_n->m = p_l->m;
			p_n->r = p_l->r;
			p_n->g = p_l->g;
			p_n->b = p_l->b;

			// Reset current liquid pixel to M_VOID
			p_l->m = M_VOID;
			sample_pixel(p_l);

			// Swap the liquid pixel pointer to new one
			m_liquid[i] = p_n;
		}

		// Run liquid update (collisions)
		if (p_n2 != nullptr)
		{
			// Both convert to obisidian if this was M_WATER <-> M_LAVA collision
			if (p_l->m == M_WATER && p_n2->m == M_LAVA || p_l->m == M_LAVA && p_n2->m == M_WATER)
			{
				// Convert lava to obisidian
				p_n2->m = M_OBSIDIAN;
				sample_pixel(p_n2);
			}
		}
	}
}

void Level::set_config(LevelConfig cfg)
{
	m_cfg = cfg;
}

LevelConfig & Level::get_config()
{
	return m_cfg;
}