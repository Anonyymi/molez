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
	m_fluid()
{

}

Level::~Level()
{

}

void Level::gen()
{
	// Reset + resize
	m_width = m_cfg.width;
	m_height = m_cfg.height;
	m_fluid.clear();
	m_bitmap.clear();
	m_bitmap.resize(m_width * m_height);

	// Generate level
	for (int32_t y = 0; y < m_height; y++)
	{
		for (int32_t x = 0; x < m_width; x++)
		{
			// Get pixel at x,y
			auto * p = &m_bitmap[x + y * m_width];

			// Init pixel
			p->x = x;
			p->y = y;
			p->n = 0;
			p->m = M_VOID;
			p->t = T_NULL;
			p->argb = 0x00000000;

			// Gen noise value at x,y in range 0..1
			float n_val = m_simplex.noise(x * m_cfg.n_scale, y * m_cfg.n_scale);
			n_val += 1.0f;
			n_val *= 0.5f;

			// Re-scaled noise value at x,y in rage 0..255 + set pixel value
			uint8_t n_val_i = static_cast<uint8_t>(n_val * 255.0f);
			p->n = n_val_i;

			// Gen dirt & air
			if (n_val_i <= m_cfg.dirt_n)
			{
				p->m = M_SOLID;
				p->t = T_DIRT;
			}
			else
			{
				p->m = M_VOID;
				p->t = T_AIR;
			}

			// Sample texture
			samplePixel(p);
		}
	}

	// Gen objects
	genObject();

	// Gen fluids
	genFluid();

	mlibc_inf("Level::gen(%u). Level generated! Type: %u, width: %zu, height: %zu", m_cfg.seed, m_cfg.type, m_width, m_height);
}

void Level::genObject()
{
	// Gen objects
	for (size_t i = 0; i < 128; i++)
	{
		// Gen random value, range 0..255
		uint8_t r_val = static_cast<uint8_t>(RNG_DIST8(RNG));

		// Gen object on chance
		if (r_val < m_cfg.object_n)
		{
			// Get x,y
			int32_t x = RNG_DIST32(RNG) % m_width;
			int32_t y = RNG_DIST32(RNG) % m_height;

			// Get pixel at x,y
			Pixel * p = &m_bitmap[x + y * m_width];

			// Draw the object in the level
			draw(M_SOLID_ID, T_ROCK, x, y);
		}
	}
}

void Level::genFluid()
{
	// Gen water clumps
	for (size_t i = 0; i < 128; i++)
	{
		// Gen random value, range 0..255
		uint8_t r_val = static_cast<uint8_t>(RNG_DIST8(RNG));

		// Gen water on chance
		if (r_val < m_cfg.water_n)
		{
			// Get radius + x,y
			uint8_t r = static_cast<uint8_t>(RNG_DIST8(RNG) % 16) + 8;
			int32_t x = RNG_DIST32(RNG) % m_width;
			int32_t y = RNG_DIST32(RNG) % m_height;

			// Get pixel at x,y
			Pixel * p = &m_bitmap[x + y * m_width];

			// Only insert water in defined noise range
			if (p->n < m_cfg.dirt_n)
				continue;

			// Alter the level
			alter(M_FLUID, T_WATER, r, x, y, false);
		}
	}

	// Gen lava clumps
	for (size_t i = 0; i < 128; i++)
	{
		// Gen random value, range 0..255
		uint8_t r_val = static_cast<uint8_t>(RNG_DIST8(RNG));

		// Gen lava on chance
		if (r_val < m_cfg.lava_n)
		{
			// Get radius + x,y
			uint8_t r = static_cast<uint8_t>(RNG_DIST8(RNG) % 16) + 8;
			int32_t x = RNG_DIST32(RNG) % m_width;
			int32_t y = RNG_DIST32(RNG) % m_height;

			// Get pixel at x,y
			Pixel * p = &m_bitmap[x + y * m_width];

			// Alter the level
			alter(M_FLUID, T_LAVA, r, x, y, false);
		}
	}
}

void Level::regen(uint32_t seed)
{
	mlibc_inf("Level::regenerate(). Regenerating level...");

	// Re-seed noise generator(s) + set cfg seed
	m_simplex.reseed(seed);
	m_cfg.seed = seed;

	// Re-generate the level
	gen();
}

void Level::alter(Material_t m, Texture_t t, uint8_t r, int x, int y, bool edit)
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
					continue;

				// Get the pixel
				Pixel * p = &m_bitmap[j + i * m_width];

				// Do not allow altering indestructible data in non-editor mode
				if (edit == false && p->m == M_SOLID_ID)
					continue;

				// Alter it accordingly + resample
				p->m = m;
				p->t = t;
				samplePixel(p);
			}
		}
	}
}

void Level::draw(Material_t m, Texture_t t, int x, int y)
{
	// Get texture for material
	TextureManager::Texture * tex = TextureManager::load_texture(sampleTexture(t));

	// Draw the texture/material on level bitmap
	int tex_x = 0;
	int tex_y = 0;
	for (int32_t i = y; i < y + tex->height; i++)
	{
		for (int32_t j = x; j < x + tex->width; j++)
		{
			// Do not allow altering memory outside level bounds
			if (j < 0 || j >= m_width || i < 0 || i >= m_height)
			{
				// Inc texcoord x + continue;
				tex_x++;
				continue;
			}

			// Get the pixel
			Pixel * p = &m_bitmap[j + i * m_width];

			// Alter it accordingly + resample (Alpha 0x00 is transparency)
			int32_t argb = TextureManager::sample_texture(tex->file_path, tex_x, tex_y);
			uint8_t a = (argb & 0xFF000000) >> 24;
			if (a != 0x00)
			{
				p->m = m;
				p->argb = argb;
			}

			// Inc texcoord x
			tex_x++;
		}

		// Inc texcoord y
		tex_y++;
	}
}

void Level::samplePixel(Pixel * p)
{
	// Final int32_t HEX ARGB color
	int32_t argb = 0;

	// Determine texture/properties by material
	switch (p->t)
	{
		case T_NULL:		argb = 0x00000000;														break;
		case T_AIR:			argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y); break;
		case T_DIRT:		argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y); break;
		case T_ROCK:		argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y); break;
		case T_MOSS:		argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y); break;
		case T_OBSIDIAN:	argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y); break;
		case T_WATER:
		{
			argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y);

			// This is a fluid
			m_fluid.push_back(p);
		} break;
		case T_LAVA:
		{
			argb = TextureManager::sample_texture(sampleTexture(p->t), p->x, p->y);

			// This is a fluid
			m_fluid.push_back(p);
		} break;
	}

	// Alpha 0x00 is transparency
	uint8_t a = (argb & 0xFF000000) >> 24;
	if (a == 0x00)
	{
		return;
	}

	// Assign new RGB values
	p->argb = argb;
}

std::string Level::sampleTexture(Texture_t t)
{
	switch (t)
	{
		case T_AIR: return "AIR.PNG";
		case T_DIRT: return "DIRT.PNG";
		case T_ROCK:
		{
			// Gen random value, range 1..ROCK_MAX
			uint8_t r_val = static_cast<uint8_t>(RNG_DIST8(RNG)) % 3;
			r_val++;

			// Return rock texture name
			return "ROCK" + std::to_string(r_val) + ".PNG";
		} break;
		case T_MOSS: return "MOSS.PNG";
		case T_OBSIDIAN: return "OBSIDIAN.PNG";
		case T_WATER: return "WATER.PNG";
		case T_LAVA: return "LAVA.PNG";
	}

	return "NULL.PNG";
}

void Level::update(float state, float t, float dt)
{
	// Fluid physics
	size_t fluid_size = m_fluid.size();
	for (size_t i = 0; i < fluid_size; i++)
	{
		// Get fluid pixel at x,y
		Pixel * p_f = m_fluid[i];

		// If pixel is not a fluid material anymore, erase + skip it
		if (p_f->m != M_FLUID)
		{
			// Erase
			m_fluid.erase(m_fluid.begin() + i);

			// Re-align loop size and iterator value
			fluid_size--;
			i--;
			continue;
		}

		// Neighbor pixel in our bitmap if it is M_VOID
		Pixel * p_n = nullptr;

		// Neighbor pixel in our bitmap if it is M_FLUID
		Pixel * p_nf = nullptr;

		// Check pixel below
		if ((p_f->y + 1) < m_height)
		{
			p_n = &m_bitmap[p_f->x + (p_f->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_nf = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+left
		if (p_n == nullptr && (p_f->y + 1) < m_height && (p_f->x - 1) < m_width)
		{
			p_n = &m_bitmap[(p_f->x - 1) + (p_f->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_nf = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel below+right
		if (p_n == nullptr && (p_f->y + 1) < m_height && (p_f->x + 1) < m_width)
		{
			p_n = &m_bitmap[(p_f->x + 1) + (p_f->y + 1) * m_width];

			if (p_n->m != M_VOID)
			{
				p_nf = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel left
		if (p_n == nullptr && (p_f->x - 1) < m_width)
		{
			p_n = &m_bitmap[(p_f->x - 1) + p_f->y * m_width];

			if (p_n->m != M_VOID)
			{
				p_nf = p_n;
				p_n = nullptr;
			}
		}

		// Check pixel right
		if (p_n == nullptr && (p_f->x + 1) < m_width)
		{
			p_n = &m_bitmap[(p_f->x + 1) + p_f->y * m_width];

			if (p_n->m != M_VOID)
			{
				p_nf = p_n;
				p_n = nullptr;
			}
		}

		// Run fluid update
		if (p_n != nullptr)
		{
			// Copy values to new fluid pixel
			p_n->m = p_f->m;
			p_n->t = p_f->t;
			p_n->argb = p_f->argb;

			// Reset current fluid pixel to M_VOID & T_AIR
			p_f->m = M_VOID;
			p_f->t = T_AIR;
			samplePixel(p_f);

			// Swap the fluid pixel pointer to new one
			m_fluid[i] = p_n;
		}

		// Run fluid update (collisions)
		if (p_nf != nullptr && p_nf->m == M_FLUID)
		{
			// Convert lava to obisidian on lava <-> water collision
			if (p_f->t == T_LAVA && p_nf->t == T_WATER)
			{
				p_f->m = M_SOLID;
				p_f->t = T_OBSIDIAN;
				samplePixel(p_f);
			}

			// Convert lava to obisidian on water <-> lava collision
			if (p_f->t == T_WATER && p_nf->t == T_LAVA)
			{
				p_nf->m = M_SOLID;
				p_nf->t = T_OBSIDIAN;
				samplePixel(p_nf);
			}
		}
	}
}

void Level::render(float state)
{
	for (size_t i = 0; i < m_bitmap.size(); i++)
	{
		// Get pixel at x,y
		auto & p = m_bitmap[i];

		// Decode argb to rgba components
		auto a = (p.argb & 0xFF000000) >> 24;
		auto r = (p.argb & 0x00FF0000) >> 16;
		auto g = (p.argb & 0x0000FF00) >> 8;
		auto b = (p.argb & 0x000000FF);

		// Set pixel to window fbo at x,y
		DisplayManager::set_pixel(p.x, p.y, r, g, b, a, false);
	}
}

void Level::setCfg(LevelConfig cfg)
{
	m_cfg = cfg;
}

LevelConfig & Level::getCfg()
{
	return m_cfg;
}