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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "iniparser.h"

#include "config.h"
#include "log.h"

#define CFG_INVALID_FLAGS	(255)
#define CFG_INVALID_SIZE	(0)
#define CFG_INVALID_VOL		(-1.0f)
#define CFG_INVALID_FPS		(0)
#define CFG_INVALID_PATH	(NULL)

/* Since strings gathered from libiniparser are stored within the dictionary,
   we need to duplicate them to ensure we can free the dictionary after
   loading. */
char *
_cfg_duplicate_string(const char *src)
{
	char	*dst;

	if (NULL == src)
		return NULL;

	dst = calloc(strlen(src) + 1, sizeof(char));

	/* Typically unsafe, but it's coming from a sanitized source. Unless
	   libiniparser is feeding us bad data, but ¯\_(ツ)_/¯ */
	strcpy(dst, src);

	return dst;
}

cfg_t *
cfg_load(const char *fn)
{
	dictionary	*ini;
	const char	*sub_ini;
	cfg_t		*new;

	if (NULL == (ini = iniparser_load(fn)))
		return NULL;

	new = malloc(sizeof(cfg_t));

	/* Graphics Configuration */
	new->flags = iniparser_getboolean(ini, "gfx:smoothing", CFG_INVALID_FLAGS);
	new->win_width = iniparser_getint(ini, "gfx:width", CFG_INVALID_SIZE);
	new->win_height = iniparser_getint(ini, "gfx:height", CFG_INVALID_SIZE);

	/* Audio Configuration */
	new->volume = iniparser_getdouble(ini, "sfx:volume", CFG_INVALID_VOL);
	new->sample_rate = iniparser_getint(ini, "sfx:sample_rate", CFG_INVALID_SIZE);
	new->bgm_path = _cfg_duplicate_string(
		iniparser_getstring(ini, "sfx:bgm_file", CFG_INVALID_PATH)
	);
	new->menu_path = _cfg_duplicate_string(
		iniparser_getstring(ini, "sfx:menu_file", CFG_INVALID_PATH)
	);
	new->death_path = _cfg_duplicate_string(
		iniparser_getstring(ini, "sfx:death_file", CFG_INVALID_PATH)
	);

	/* Game Configuration */
	new->fps_limit = iniparser_getint(ini, "game:fps_limit", CFG_INVALID_FPS);
	new->ui_lines = iniparser_getint(ini, "game:ui_lines", CFG_INVALID_FPS);

	/* Rendering Configuration */
	new->font_path = _cfg_duplicate_string(
		iniparser_getstring(ini, "gfx:font_file", CFG_INVALID_PATH)
	);
	new->icon_path = _cfg_duplicate_string(
		iniparser_getstring(ini, "gfx:icon_file", CFG_INVALID_PATH)
	);
	new->font_scale = iniparser_getdouble(ini, "gfx:font_scale", CFG_INVALID_VOL);
	new->font_x = iniparser_getint(ini, "gfx:font_x", CFG_INVALID_SIZE);
	new->font_y = iniparser_getint(ini, "gfx:font_y", CFG_INVALID_SIZE);

	/* Map generation. */
	new->map.floor_mul = iniparser_getdouble(ini, "gen:floor_mul", CFG_INVALID_VOL);
	new->map.floor_start = iniparser_getint(ini, "gen:floor_start", CFG_INVALID_SIZE);
	new->map.room_num = iniparser_getint(ini, "gen:room_num", CFG_INVALID_SIZE);
	new->map.branch_num = iniparser_getint(ini, "gen:branch_num", CFG_INVALID_SIZE);
	new->map.room_min.x = iniparser_getint(ini, "gen:room_min_x", CFG_INVALID_SIZE);
	new->map.room_min.y = iniparser_getint(ini, "gen:room_min_y", CFG_INVALID_SIZE);
	new->map.room_max.x = iniparser_getint(ini, "gen:room_max_x", CFG_INVALID_SIZE);
	new->map.room_max.y = iniparser_getint(ini, "gen:room_max_y", CFG_INVALID_SIZE);

	info("Config loaded: %s\n", fn);

	if (NULL != (sub_ini = iniparser_getstring(ini, "cfg:include", NULL)))
		cfg_load_and_merge(new, sub_ini);

	iniparser_freedict(ini);
	return new;
}

int
cfg_load_and_merge(cfg_t *old, const char *fn)
{
	cfg_t	*new;

	if (NULL == (new = cfg_load(fn)))
		return 1;

	/* Take new values if they are not invalid. */
	if (CFG_INVALID_FLAGS != new->flags)
		old->flags = new->flags;
	
	if (new->win_width > CFG_INVALID_SIZE)
		old->win_width = new->win_width;

	if (new->win_height > CFG_INVALID_SIZE)
		old->win_height = new->win_height;

	if (CFG_INVALID_PATH != new->font_path) {
		if (NULL != old->font_path)
			free(old->font_path);
		old->font_path = new->font_path;
		new->font_path = NULL;
	}

	if (CFG_INVALID_PATH != new->icon_path) {
		if (NULL != old->icon_path)
			free(old->icon_path);
		old->icon_path = new->icon_path;
		new->icon_path = NULL;
	}

	if (CFG_INVALID_VOL != new->font_scale)
		old->font_scale = fmaxf(new->font_scale, 0.0f);

	if (new->font_x > CFG_INVALID_SIZE)
		old->font_x = new->font_x;

	if (new->font_y > CFG_INVALID_SIZE)
		old->font_y = new->font_y;

	if (CFG_INVALID_VOL != new->volume)
		old->volume = fminf(fmaxf(new->volume, 0.0f), 1.0f);

	if (CFG_INVALID_FPS != new->sample_rate)
		old->sample_rate = new->sample_rate;

	if (CFG_INVALID_PATH != new->bgm_path) {
		if (NULL != old->bgm_path)
			free(old->bgm_path);
		old->bgm_path = new->bgm_path;
		new->bgm_path = NULL;
	}

	if (CFG_INVALID_PATH != new->menu_path) {
		if (NULL != old->menu_path)
			free(old->menu_path);
		old->menu_path = new->menu_path;
		new->menu_path = NULL;
	}

	if (CFG_INVALID_PATH != new->death_path) {
		if (NULL != old->death_path)
			free(old->death_path);
		old->death_path = new->death_path;
		new->death_path = NULL;
	}

	if (CFG_INVALID_FPS != new->fps_limit)
		old->fps_limit = new->fps_limit;

	if (CFG_INVALID_FPS != new->ui_lines)
		old->ui_lines = new->ui_lines;

	if (CFG_INVALID_VOL != new->map.floor_mul)
		old->map.floor_mul = fmaxf(new->map.floor_mul, 0.0f);

	if (new->map.floor_start > CFG_INVALID_SIZE)
		old->map.floor_start = new->map.floor_start;

	if (new->map.room_num > CFG_INVALID_SIZE)
		old->map.room_num = new->map.room_num;

	if (new->map.branch_num > CFG_INVALID_SIZE)
		old->map.branch_num = new->map.branch_num;

	if (new->map.room_min.x > CFG_INVALID_SIZE)
		old->map.room_min.x = new->map.room_min.x;

	if (new->map.room_min.y > CFG_INVALID_SIZE)
		old->map.room_min.y = new->map.room_min.y;

	if (new->map.room_max.x > CFG_INVALID_SIZE)
		old->map.room_max.x = new->map.room_max.x;

	if (new->map.room_max.y > CFG_INVALID_SIZE)
		old->map.room_max.y = new->map.room_max.y;

	return 0;
}

void
cfg_free(cfg_t *cfg)
{
	if (NULL != cfg->font_path)
		free(cfg->font_path);
	if (NULL != cfg->icon_path)
		free(cfg->icon_path);
	free(cfg);
}
