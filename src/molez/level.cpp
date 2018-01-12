#include "level.h"
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "math.h"

Level::Level(LevelConfig config) :
	m_cfg(config),
	m_simplex(m_cfg.seed),
	m_bitmap(m_cfg.width * m_cfg.height),
	m_liquid()
{

}

Level::Level():
	m_liquid()
{
}

Level::~Level()
{

}



void Level::setConfig(LevelConfig cfg){
	m_cfg = cfg;

	m_bitmap.resize(m_cfg.width * m_cfg.height);
	m_simplex.reseed(m_cfg.seed);

}

void Level::generate()
{
	// Reset liquids
	m_liquid.clear();

	// Iterate through each pixel to generate level geometry
	for (int32_t i = 0; i < m_cfg.height; i++)
	{
		for (int32_t j = 0; j < m_cfg.width; j++)
		{
			// Get pixel at x,y
			auto * pixel = &m_bitmap[j + i * m_cfg.width];

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

			// Select material
			if (n_value <= 0.33f)
			{
				pixel->m = M_VOID;

				// Is this liquid or not?
				int rng_val = RNG_DIST(RNG);
				if (rng_val < m_cfg.n_water)
				{
					pixel->m = M_WATER;
				}
			}
			else if (n_value >= 0.33f)
			{
				pixel->m = M_DIRT;
			}

			// Sample texture rgb for the pixel
			sample_pixel(pixel);
		}
	}

	mlibc_inf("Level::generate(). Level generated! Type: %u, width: %zu, height: %zu", m_cfg.type, m_cfg.width, m_cfg.height);
}

void Level::regenerate(uint32_t seed)
{
	mlibc_inf("Level::regenerate(%u). Regenerating level...", seed);

	// Re-seed noise generator(s)
	m_simplex.reseed(seed);

	// Re-generate the level
	generate();
}

void Level::sample_pixel(Pixel * pixel)
{
	// Final int32_t HEX ARGB color
	int32_t argb = 0;

	// Determine texture/properties by material
	switch (pixel->m)
	{
		case M_DIRT:
		{
			argb = TextureManager::sample_texture("DIRT.PNG", pixel->x, pixel->y);
		} break;
		case M_OBSIDIAN:
		{
			argb = TextureManager::sample_texture("OBSIDIAN.PNG", pixel->x, pixel->y);
		} break;
		case M_WATER:
		{
			argb = TextureManager::sample_texture("WATER.PNG", pixel->x, pixel->y);

			// This is a liquid
			m_liquid.push_back(pixel);
		} break;
		case M_LAVA:
		{
			argb = TextureManager::sample_texture("LAVA.PNG", pixel->x, pixel->y);

			// This is a liquid
			m_liquid.push_back(pixel);
		} break;
	}

	// Assign new RGB values
	pixel->r = (argb & 0x00FF0000) >> 16;
	pixel->g = (argb & 0x0000FF00) >> 8;
	pixel->b = (argb & 0x000000FF);
}

void Level::alter(Material_t m, int32_t r, int32_t x, int32_t y)
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
				if (j < 0 || j >= m_cfg.width || i < 0 || i >= m_cfg.height)
				{
					continue;
				}

				// Get the pixel
				Pixel * pixel = &m_bitmap[j + i * m_cfg.width];

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
		if ((p_l->y + 1) < m_cfg.height)
		{
			p_n = &m_bitmap[p_l->x + (p_l->y + 1) * m_cfg.width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+left
		if (p_n == nullptr && (p_l->y + 1) < m_cfg.height && (p_l->x - 1) < m_cfg.width)
		{
			p_n = &m_bitmap[(p_l->x - 1) + (p_l->y + 1) * m_cfg.width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+right
		if (p_n == nullptr && (p_l->y + 1) < m_cfg.height && (p_l->x + 1) < m_cfg.width)
		{
			p_n = &m_bitmap[(p_l->x + 1) + (p_l->y + 1) * m_cfg.width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel left
		if (p_n == nullptr && (p_l->x - 1) < m_cfg.width)
		{
			p_n = &m_bitmap[(p_l->x - 1) + p_l->y * m_cfg.width];

			if (p_n->m != M_VOID)
			{
				p_n2 = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel right
		if (p_n == nullptr && (p_l->x + 1) < m_cfg.width)
		{
			p_n = &m_bitmap[(p_l->x + 1) + p_l->y * m_cfg.width];

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
			p_l->r = 0;
			p_l->g = 0;
			p_l->b = 0;

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