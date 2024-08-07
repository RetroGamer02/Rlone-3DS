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

#include <stdarg.h>
#include <string.h>

#include <SDL_image.h>

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "renderer_util.h"
#include "renderer.h"

#include "object_player.h"
#include "object_npc.h"
#include "object.h"

#include "log.h"

static rend_t	  renderer;
static int		  renderer_initialized;
static char		**renderer_lines;
static int		  renderer_lines_len;
static size_t	  renderer_line_len;

rend_t *
rend_init(cfg_t *cfg)
{
#ifdef DEBUG
	SDL_RendererInfo	 info;
#endif
	SDL_Surface			*font;
	SDL_Surface			*icon;
	int					 i;

	renderer.cfg = cfg;

	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
		fatal("Unable to initialize SDL: %s\n", SDL_GetError());

    // Don't ask the WM to disable the compositor, as it's not needed.
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

	renderer.win = SDL_CreateWindow(
		"Rlone: A Rogue Clone",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		cfg->win_width, cfg->win_height,
#ifdef VITABUILD
		SDL_WINDOW_SHOWN
#elif __3DS__
		SDL_WINDOW_SHOWN
#else
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
#endif
	);

	if (NULL != (icon = IMG_Load(cfg->icon_path))) {
		SDL_SetWindowIcon(renderer.win, icon);
		SDL_FreeSurface(icon);
	}

	SDL_SetHintWithPriority(
		SDL_HINT_RENDER_SCALE_QUALITY,
		cfg->flags & CFG_SMOOTHING ? "1" : "0",
		SDL_HINT_OVERRIDE
	);

	if (NULL == (font = IMG_Load(cfg->font_path)))
		fatal("Unable to initialize renderer: %s\n", SDL_GetError());

	renderer.backbuffer = SDL_CreateRenderer(
		renderer.win,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
	);

#ifdef DEBUG
	SDL_GetRendererInfo(renderer.backbuffer, &info);
	renderer.supports_targets = info.flags & SDL_RENDERER_TARGETTEXTURE;
	debug("Renderer: %s\n"
		  "\tAllows Offscreen: %s\n"
		  "\tMax Texture Size: %d, %d\n",
		  info.name,
		  renderer.supports_targets ? "yes" : "no",
		  info.max_texture_width, info.max_texture_height);
#endif

	renderer.font = SDL_CreateTextureFromSurface(renderer.backbuffer, font);
	SDL_QueryTexture(
		renderer.font,
		NULL, NULL,
		&renderer.font_w, &renderer.font_h
	);

	rend_dialog_setrend(&renderer);
	rend_util_setrend(&renderer);

	/* Setup the renderer update lines. */
	renderer_line_len = cfg->win_width / (cfg->font_x * cfg->font_scale);
	renderer_lines_len = cfg->ui_lines - 1;
	renderer_lines = malloc(sizeof(char *) * renderer_lines_len);
	for (i = 0; i < renderer_lines_len; i++) {
		renderer_lines[i] = calloc(
			sizeof(char),
			renderer_line_len + 1
		);
	}

	SDL_FreeSurface(font);
	renderer_initialized = 1;
	return &renderer;
}

SDL_Rect
_rend_get_dst_size()
{
	SDL_Rect	dst_pos = (SDL_Rect){
		0,
		0,
		renderer.cfg->font_x * renderer.cfg->font_scale,
		renderer.cfg->font_y * renderer.cfg->font_scale
	};

	return dst_pos;
}

void
rend_set_dialog(dialog_t *diag)
{
	renderer.cur_dialog = diag;
}

void
rend_text_append(const char *fmt, ...)
{
	object_t	*ticker;
	va_list		 va;
	int			 i;

	/* Reset ticker if added data. */
	if (NULL != (ticker = object_first(OBJ_TICKER)))
		ticker->us_a = 0;

	/* Move all the text down 1 row, discarding last. */
	for (i = renderer_lines_len - 2; i >= 0; i--) {
		strncpy(
			renderer_lines[i + 1],
			renderer_lines[i],
			sizeof(char) * renderer_line_len
		);
	}

	va_start(va, fmt);
	vsnprintf(renderer_lines[0], renderer_line_len, fmt, va);
	va_end(va);
}

void
__rend_draw_ent(const rend_entity_t *ent, SDL_Rect dst_pos)
{
	SDL_Rect	chr_pos;

	chr_pos = rend_get_char(ent->c);

	SDL_SetTextureColorMod(
		renderer.font,
		ent->r,
		ent->g,
		ent->b
	);

	/* Clear the map tile, then draw the object character. */
	if (ent->c != 20)
		SDL_RenderFillRect(renderer.backbuffer, &dst_pos);

	SDL_RenderCopy(
		renderer.backbuffer,
		renderer.font,
		&chr_pos, 
		&dst_pos
	);
}

void
_rend_draw_entity(enum rend_entity et, SDL_Rect dst_pos)
{
	__rend_draw_ent(
		rend_entity_get(et),
		dst_pos
	);
}

void
_rend_draw_object(object_t *obj, SDL_Rect dst_pos)
{
	const rend_entity_t	*ent;

	if (obj->enabled && !OBJ_REND_HAS(obj, OBJ_REND_DISABLED)) {
		ent = rend_entity_get(obj->ent);
		__rend_draw_ent(ent, dst_pos);
	}
}

SDL_Rect
_rend_draw_number(const uint8_t num_digits, int num, SDL_Rect dst_pos)
{
	/* We can assume 3, since it's an 8-bit number. */
#ifdef _WIN32
	/* C++ doesn't like VAs, and to be fair neither to I.
	   But I was lazy, okay? */
	uint8_t		*digits,
#else
	uint8_t		digits[num_digits],
#endif
				digit_count = 0;
	SDL_Rect	chr_pos;

#ifdef _WIN32
	/* Guess windows gets to be slower than *nix. */
	digits = malloc(num_digits);
#endif

	/* Gather the digits, by modding and dividing. */
	while (num > 0 && digit_count < num_digits) {
		digits[digit_count++] = num % 10;
		num /= 10;
	}

	/* Pad out the rest with 0s. */
	while (digit_count < num_digits)
		digits[digit_count++] = 0;

	SDL_SetTextureColorMod(renderer.font, 255, 255, 255);

	/* Render out the digits in order. */
	while (digit_count-- > 0) {
		chr_pos = rend_get_char('0' + digits[digit_count]);

		SDL_RenderCopy(
			renderer.backbuffer,
			renderer.font,
			&chr_pos,
			&dst_pos
		);

		dst_pos.x += dst_pos.w;
	}

#ifdef _WIN32
	free(digits);
#endif

	/* Return the resulting position for less math to be required. */
	return dst_pos;
}

void
_rend_draw_lines(SDL_Rect dst_pos)
{
	SDL_Rect			 chr_pos;
	int					 i;
	size_t				 line_len,
						 line_i;
	const rend_entity_t	*ent;
	char				 c;

	for (i = 0; i < renderer_lines_len; i++) {
		line_len = strlen(renderer_lines[i]);
		if (line_len == 0)
			continue;

		/* Walk render the line. */
		for (line_i = 0; line_i < line_len; line_i++) {
			c = renderer_lines[i][line_i];
			if (NULL != (ent = rend_entity_for(c)))
				__rend_draw_ent(ent, dst_pos);
			else {
				chr_pos = rend_get_char(c);

				SDL_SetTextureColorMod(renderer.font, 255, 255, 255);
				SDL_RenderCopy(
					renderer.backbuffer,
					renderer.font,
					&chr_pos,
					&dst_pos
				);
			}

			dst_pos.x += dst_pos.w;
		}

		dst_pos.x = 0;
		dst_pos.y += dst_pos.h;
	}
}

int
rend_handle_frame(map_t *map)
{
	SDL_Rect			 chr_pos;
	int					 dst_walk	= 0,
						 dst_width	= 0,
						 dst_height	= 0,
						 w_off		= 0,
						 h_off		= 0,
						 map_walk	= 0,
						 dst_last_y	= 0,
						 map_y		= map->size.y;
	size_t				 obj_i		= 0;
	object_t			*obj;
	SDL_Rect			 ui_pos,
						dst_pos = _rend_get_dst_size();
	object_t			*player,
						*inspecter;
	const rend_entity_t	*ent;

	if (!renderer_initialized)
		return -1;

	inspecter = object_first(OBJ_INSPECTER);
	if(NULL == (player = object_first(OBJ_PLAYER)))
		warn("No player provided to renderer.\n", NULL);

	/* Calculate the map_walk using the map offsets. */
	map_walk = (map->size.w * map->size.y) + map->size.x;

	/* Cutoff any character that may not be fully rendered. */
	w_off = renderer.cfg->win_width % dst_pos.w;
	h_off = renderer.cfg->win_height % dst_pos.h;

	dst_width = renderer.cfg->win_width - w_off;
	dst_height = renderer.cfg->win_height - h_off;

	/* Cut off the last two characters for UI rendering. */
	dst_height -= dst_pos.h * renderer.cfg->ui_lines;

	/* Setup the UI rect. */
	ui_pos = (SDL_Rect){
		0,
		dst_height,
		renderer.cfg->win_width,
		dst_pos.h * (1 + renderer.cfg->ui_lines)
	};

	/* Divide the offsets by 2 in order to center the rendering view. */
	w_off /= 2;
	h_off /= 2;

	/* Adjust the rendering view to match the size of the map, if smaller. */
	if ((map->size.w * dst_pos.w) < dst_width) {
		w_off += (dst_width - (map->size.w * dst_pos.w)) / 2;
		dst_width = (map->size.w * dst_pos.w);
	}

	if ((map->size.h * dst_pos.h) < dst_height) {
		h_off += (dst_height - (map->size.h * dst_pos.h)) / 2;
		dst_height = (map->size.h * dst_pos.h) + h_off;
	}

	SDL_SetRenderDrawColor(
		renderer.backbuffer,
		map->bg_r,
		map->bg_g,
		map->bg_b,
		255
	);

	SDL_RenderClear(renderer.backbuffer);

	/* TODO: Maybe worth separating these into separate functions, but honestly
	   I'm not to stressed about it. */
	if (NULL != renderer.cur_dialog) {
		/* Short circuit the map rendering for the dialog. */
		rend_dialog_render(renderer.cur_dialog);
	} else {
		dst_last_y = h_off;

		/* Map rendering. */
		while (dst_pos.y < dst_height) {
			dst_pos.x = w_off + dst_walk % dst_width;
			dst_pos.y = h_off + dst_pos.h * (dst_walk / dst_width);

			/* Skip to the next map row if we've moved to the next dst row. */
			if (dst_last_y != dst_pos.y)
				map_walk = (map->size.w * (++map_y)) + map->size.x;

			if (NULL != (ent = rend_entity_for(map->buffer[map_walk]))) {
				SDL_SetTextureColorMod(
					renderer.font,
					ent->r,
					ent->g,
					ent->b
				);
			}
			else if (map->buffer[map_walk] == MAP_WALL) {
				SDL_SetTextureColorMod(
					renderer.font,
					map->w_r,
					map->w_g,
					map->w_b
				);
			}
			else {
				SDL_SetTextureColorMod(
					renderer.font,
					map->f_r,
					map->f_g,
					map->f_b
				);
			}

			chr_pos = rend_get_char(map->buffer[map_walk++]);
		
			if (dst_pos.y >= dst_height)
				break; /* TODO: Maybe find a more elegant solution, but honestly
						  this is completely acceptable. */

			SDL_RenderCopy(renderer.backbuffer, renderer.font, &chr_pos, &dst_pos);

			dst_walk += dst_pos.w;
			dst_last_y = dst_pos.y;
		}

		/* Render the objects on screen. */
		for (obj_i = 0; obj_i < map->objects_len; obj_i++) {
			obj = map->objects + obj_i;

			/* Don't render off-screen objects. */
			if (obj->x < map->size.x || obj->x > (map->size.x + map->size.w)
			 || obj->y < map->size.y || obj->y > (map->size.y + map->size.h))
				continue;

			/* Calculate the destination for this game object. */
			dst_pos.x = (w_off + (obj->x * dst_pos.w))
					  - (map->size.x * dst_pos.w);
			dst_pos.y = h_off + (obj->y * dst_pos.h)
					  - (map->size.y * dst_pos.h);

			_rend_draw_object(obj, dst_pos);
		}

		/* Re-render mobs. */
		for (obj_i = 0; obj_i < map->objects_len; obj_i++) {
			obj = map->objects + obj_i;

			/* Don't render off-screen objects. */
			if (obj->x < map->size.x || obj->x > (map->size.x + map->size.w)
			 || obj->y < map->size.y || obj->y > (map->size.y + map->size.h))
				continue;

			if (obj->type != OBJ_NPC_ZOMBIE
			 && obj->type != OBJ_NPC_STATUE
			 && obj->type != OBJ_NPC_GOBLIN)
				continue;

			if (OBJ_IS(obj, OBJ_NPC_DEAD))
				continue;

			/* Calculate the destination for this game object. */
			dst_pos.x = (w_off + (obj->x * dst_pos.w))
					  - (map->size.x * dst_pos.w);
			dst_pos.y = h_off + (obj->y * dst_pos.h)
					  - (map->size.y * dst_pos.h);

			_rend_draw_object(obj, dst_pos);
		}

		/* Re-render the player so that it's always on top. */
		/* Calculate the destination for the player. */
		dst_pos.x = (w_off + (player->x * dst_pos.w))
				  - (map->size.x * dst_pos.w);
		dst_pos.y = h_off + (player->y * dst_pos.h)
				  - (map->size.y * dst_pos.h);

		_rend_draw_object(player, dst_pos);

		/* Re-render slashes. */
		for (obj_i = 0; obj_i < map->objects_len; obj_i++) {
			obj = map->objects + obj_i;

			/* Don't render off-screen objects. */
			if (obj->x < map->size.x || obj->x > (map->size.x + map->size.w)
			 || obj->y < map->size.y || obj->y > (map->size.y + map->size.h))
				continue;

			if (obj->type != OBJ_SLASH && obj->type != OBJ_PROJECTILE_ARROW)
				continue;

			/* Calculate the destination for this game object. */
			dst_pos.x = (w_off + (obj->x * dst_pos.w))
					  - (map->size.x * dst_pos.w);
			dst_pos.y = h_off + (obj->y * dst_pos.h)
					  - (map->size.y * dst_pos.h);

			_rend_draw_object(obj, dst_pos);
		}

		/* Re-render the inspecter as well if it's present. */
		if (NULL != inspecter) {
			dst_pos.x = (w_off + (inspecter->x * dst_pos.w))
					  - (map->size.x * dst_pos.w);
			dst_pos.y = h_off + (inspecter->y * dst_pos.h)
					  - (map->size.y * dst_pos.h);

			_rend_draw_object(inspecter, dst_pos);
		}

		SDL_SetRenderDrawColor(renderer.backbuffer, 0, 0, 0, 0);
		SDL_RenderFillRect(renderer.backbuffer, &ui_pos);

		/* Action lines, indicating last actions as a sentence. */
		dst_pos.y = ui_pos.y;
		dst_pos.x = 0;
		_rend_draw_lines(dst_pos);

		/* Handle rendering the HP, AC, gold, etc of the player. */
		dst_pos.y = ui_pos.y + (dst_pos.h * (renderer.cfg->ui_lines - 1));
		dst_pos.x = 0;

		/* This is really dumb and clumsy. I know. */
		_rend_draw_entity(ENT_HEART, dst_pos);
		dst_pos.x += dst_pos.w;
		dst_pos = _rend_draw_number(2, player->hp, dst_pos);

		dst_pos.x += dst_pos.w;
		_rend_draw_entity(object_player_get_armor_entity(), dst_pos);
		dst_pos.x += dst_pos.w;
		dst_pos = _rend_draw_number(2, player->ac, dst_pos);

		dst_pos.x += dst_pos.w;
		_rend_draw_entity(ENT_HEALTH_POTION, dst_pos);
		dst_pos.x += dst_pos.w;
		dst_pos = _rend_draw_number(2, object_player_get_health_potions(), dst_pos);

		dst_pos.x += dst_pos.w;
		_rend_draw_entity(ENT_COIN, dst_pos);
		dst_pos.x += dst_pos.w;
		dst_pos = _rend_draw_number(5, player->gold, dst_pos);

		if (OBJ_HAS(player, OBJ_PLAYER_BOW) || player->ammo > 0) {
			dst_pos.x += dst_pos.w;
			_rend_draw_entity(ENT_ARROW, dst_pos);
			dst_pos.x += dst_pos.w;
			dst_pos = _rend_draw_number(3, player->ammo, dst_pos);
		}

		dst_pos.x = renderer.cfg->win_width - (2 * dst_pos.w);
		_rend_draw_entity(object_player_get_sword_entity(), dst_pos);
		dst_pos.x += dst_pos.w;
		_rend_draw_entity(object_player_get_bow_entity(), dst_pos);
	}

	SDL_RenderPresent(renderer.backbuffer);
	return 0;
}

SDL_Rect
rend_get_tile_dimensions()
{
	SDL_Rect dst_size = _rend_get_dst_size();
	
	return (SDL_Rect){
		0, 0,
		renderer.cfg->win_width / dst_size.w,
		renderer.cfg->win_height / dst_size.h - renderer.cfg->ui_lines
	};
}

void
rend_close()
{
	int	i;

	for (i = 0; i < renderer_lines_len; i++)
		free(renderer_lines[i]);
	free(renderer_lines);

	SDL_DestroyTexture(renderer.font);
	SDL_DestroyRenderer(renderer.backbuffer);
	SDL_DestroyWindow(renderer.win);
	SDL_Quit();

	renderer_initialized = 0;
}
