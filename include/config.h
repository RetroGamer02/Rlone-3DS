/*
 * This file is part of Rlone.
 *
 * Rlone is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Rlone is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Rlone. If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef RLONE_CONFIG_H
#define RLONE_CONFIG_H

#ifdef VITABUILD
#define CFG_AUTORUN_PATH	("app0:/autorun.ini")
#elif __3DS__
#define CFG_AUTORUN_PATH	("romfs:/autorun.ini")
#else
#define CFG_AUTORUN_PATH	("res/cfg/autorun.ini")
#endif
#define CFG_DEFAULT_FONT	("res/tex/font.tga")
#define CFG_DEFAULT_ICON	("res/tex/icon.tga")
#define CFG_DEFAULT_VOL		(1.0f)
#define	CFG_DEFAULT_FPS		(60)

#include <stdint.h>

#include <SDL.h>

#define CFG_NOSMOOTHING	(0x0)
#define CFG_SMOOTHING	(0x1)

typedef struct {
	int			floor_start;
	float		floor_mul;
	int			branch_num;
	int			room_num;
	SDL_Point	room_min;
	SDL_Point	room_max;
} cfg_map_t;

typedef struct {
	uint8_t		 flags;

	/* Window Configuration */
	uint16_t	 win_width,
				 win_height;

	/* Game Configuration */
	int16_t		 fps_limit;
	int8_t		 ui_lines;

	/* Sound Configuration */
	float		 volume;
	int			 sample_rate;
	char		*bgm_path;
	char		*menu_path;
	char		*death_path;

	/* Rendering Configuration */
	char		*icon_path;
	char		*font_path;
	float		 font_scale;
	uint8_t		 font_x,
				 font_y;

	/* Map generation information. */
	cfg_map_t	 map;
} cfg_t;

#ifdef CFG_LOAD_DEFAULT
static cfg_t	default_config = {
	CFG_NOSMOOTHING,
	800,
	600,
	CFG_DEFAULT_FPS,
	4,
	CFG_DEFAULT_VOL,
	22050,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	1.0f,
	10,
	12,
	/* cfg_map_t */
	{ 0, 0, 0, 0, { 0, 0 }, { 0, 0 }  }
};
#endif

/* For handling the actual loading logic of a config file. */
cfg_t	*cfg_load(const char *);

/* For easily loading multiple config files, although that will be likely
   unnecessary due to the simplicity of the game. */
int		 cfg_load_and_merge(cfg_t *, const char *);

void	 cfg_free(cfg_t *);

#endif
