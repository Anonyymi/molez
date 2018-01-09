#include "level.h"
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "math.h"

Level::Level(
	Level_t type,
	size_t width,
	size_t height,
	uint32_t seed
) :
	m_type(type),
	m_width(width),
	m_height(height),
	m_nGenSimplex(seed),
	m_bitmap(m_width * m_height),
	m_liquid()
{

}

Level::~Level()
{

}

void Level::generate()
{
	// Reset liquids
	m_liquid.clear();

	// Noise scale
	float n_scale = 0.025f;

	for (size_t i = 0; i < m_height; i++)
	{
		for (size_t j = 0; j < m_width; j++)
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
			float n_value = m_nGenSimplex.noise(j * n_scale, i * n_scale);
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
				if (rng_val < 40)
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

	mlibc_inf("Level::generate(). Level generated! Type: %u, width: %zu, height: %zu", m_type, m_width, m_height);
}

void Level::regenerate(uint32_t seed)
{
	mlibc_inf("Level::regenerate(%u). Regenerating level...", seed);

	// Re-seed noise generator(s)
	m_nGenSimplex.reseed(seed);

	// Re-generate level
	generate();
}

void Level::sample_pixel(Pixel * pixel)
{
	int32_t argb = 0;

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

	pixel->r = (argb & 0x00FF0000) >> 16;
	pixel->g = (argb & 0x0000FF00) >> 8;
	pixel->b = (argb & 0x000000FF);
}

void Level::alter(Material_t material, int32_t radius, int32_t x, int32_t y)
{
	// Calculate start coords
	int32_t x_start = x - radius;
	int32_t y_start = y - radius;

	// Iterate over the area we want to alter
	for (size_t i = y_start; i < y_start + radius * 2; i++)
	{
		for (size_t j = x_start; j < x_start + radius * 2; j++)
		{
			// Get distance from center to current pos
			int32_t dist = std::sqrt(((j - x) * (j - x)) + ((i - y) * (i - y)));

			// Alter only if we are within the given radius
			if (dist < radius)
			{
				// Do not allow altering memory outside level bounds
				if (j < 0 || j >= m_width || i < 0 || i >= m_height)
				{
					continue;
				}

				// Get the pixel
				Pixel * pixel = &m_bitmap[j + i * m_width];

				// Alter it accordingly
				pixel->m = material;
				sample_pixel(pixel);
			}
		}
	}
}

void Level::render()
{
	for (size_t i = 0; i < m_height; i++)
	{
		for (size_t j = 0; j < m_width; j++)
		{
			// Get pixel at x,y
			auto & pixel = m_bitmap[j + i * m_width];

			// Set pixel to screen fbo at x,y
			DisplayManager::set_pixel(j, i, pixel.r, pixel.g, pixel.b);
		}
	}
}

void Level::update()
{
	// Liquid physics
	for (size_t i = 0; i < m_liquid.size(); i++)
	{
		// Get liquid pixel at x,y
		Pixel * p_l = m_liquid[i];

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
				p_n = nullptr;
			}
		}

		// Check pixel below+right
		if (p_n == nullptr && (p_l->y + 1) < m_height && (p_l->x + 1) < m_width)
		{
			p_n = &m_bitmap[(p_l->x + 1) + (p_l->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
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