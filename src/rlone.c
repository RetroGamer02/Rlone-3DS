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

#ifdef VITABUILD
#include <psp2/kernel/processmgr.h>

#define TITLE_BG_FILE	"app0:/tex/title_bg.tga"
#elif __3DS__
#include <3ds.h>
#define TITLE_BG_FILE	"romfs:/tex/title_bg.tga"
#else
#define TITLE_BG_FILE	"res/tex/title_bg.tga"
#endif

#define CFG_LOAD_DEFAULT
#include "object_player.h"
#include "renderer.h"
#include "config.h"
#include "audio.h"
#include "input.h"
#include "map.h"
#include "log.h"

#ifdef _WIN32
#include "libraries.h"

/* ????? Thanks SDL2, apparently y'all don't like Windows either. */
#undef main

/* Implement WinMain to call main. */
int
WinMain(void)
{
	main();
}
#endif

static cfg_t		*config = &default_config;
static rend_t		*renderer;
#ifdef NODEBUG
static dialog_t		*main_menu;
#endif
static dialog_t		*pause_menu;
static dialog_t		*control_menu;
static dialog_t		*legend_menu;
static dialog_t		*next_floor_menu;
static dialog_t		*death_menu;
static dialog_t		*restart_menu;
static map_t		*cur_map;
static object_t		*player;
static SDL_Point	 last_offset;
static int			 cur_floor;

int game_update(float);
#ifdef __3DS__
int
main(int argc, char *argv[])
{
	//gfxInitDefault();

	//consoleInit(GFX_TOP, NULL);

    Result rc = romfsInit();
	if (rc)
		printf("romfsInit: %08lX\n", rc);
		
#else
int
main(void)
{
#endif
	int				quit = 0;
	int				ticks, last_ticks;
	float			dt;

#if defined(NODEBUG) && !defined(VITABUILD)
	printf("Welcome to...\n"
		   " -__ /\\   ,,                   \n"
		   "   || \\,  ||                   \n"
		   "  /|| /   ||  /'\\\\ \\\\/\\\\  _-_  \n"
		   "  \\||/-   || || || || || || \\\\ \n"
		   "   ||  \\  || || || || || ||/   \n"
		   " _---_-|, \\\\ \\\\,/  \\\\ \\\\ \\\\,/  \n"
		   "=============v" VSN "==============\n");
									  
#endif

	log_init(NULL);

	if (cfg_load_and_merge(config, CFG_AUTORUN_PATH))
		fatal("Unable to load autorun.ini, unable to start.", NULL);

#ifdef DEBUG
	debug("Rlone configuration loaded:\n"
		  "\tWINDOW: %d, %d\n"
		  "\t   FPS: %d\n"
		  "\tVOLUME: %f\n"
		  "\t  FONT: %s\n"
		  "\t     X: %d\n"
		  "\t     Y: %d\n"
		  "\t SCALE: %f\n"
		  "\tSMOOTH: %d\n",
		  config->win_width, config->win_height,
		  config->fps_limit,
		  config->volume,
		  config->font_path,
		  config->font_x,
		  config->font_y,
		  config->font_scale,
		  config->flags & CFG_SMOOTHING);
#endif

	renderer = rend_init(config);

	audio_init(config);
	audio_set_bgm(AUD_BGM);

#ifdef NODEBUG
	/* Create the title screen. */
	main_menu = rend_dialog_create(
		"Rl\x1Ene\n\nA Rogue Clone", TITLE_BG_FILE
	);

	rend_set_dialog(main_menu);
	audio_set_bgm(AUD_MENU);
#endif

#ifdef VITABUILD
	control_menu = rend_dialog_create(
		"Controls\n\n"
		"     DPad Movement             \n"
		"\xFE + DPad Melee Attack        \n"
		"\xFC + DPad Ranged Attack       \n"
		"\xFD + DPad Interact            \n"
		"       \xFB Use Health Potion   \n"
		"       \xF9 Inspect Mode        \n"
		"       \xFE Exit Inspect Mode   \n"
		"       \xFA Pause/Unpause       \n"
		"       \xFD Inspect/Exit Dialog \n"
		"   \xF8 + \xFD Show This Dialog    \n"
		"   \xF8 + \xFE Show Melee Weapon   \n"
		"   \xF8 + \xFC Show Ranged Weapon  \n"
		"   \xF8 + \xF7 Reroll Color Palette",
		NULL
	);
#elif __3DS__
	control_menu = rend_dialog_create(
		"Controls\n\n"
		"     DPad Movement             \n"
		"   B + DPad Melee Attack        \n"
		"   X + DPad Ranged Attack       \n"
		"   A + DPad Interact            \n"
		"       Y Use Health Potion   \n"
		"       R Inspect Mode        \n"
		"       Start Exit Inspect Mode   \n"
		"       Select Pause/Unpause       \n"
		"       Start Inspect/Exit Dialog \n"
		"       L Show This Dialog    \n"
		"   Unbound Show Melee Weapon   \n"
		"   Unbound Show Ranged Weapon  \n"
		"   Unbound Reroll Color Palette",
		NULL
	);
#else
	control_menu = rend_dialog_create(
		"Controls\n\n"
		"      \x1\x2 Movement          \n"
		"[a] + \x1\x2 Melee Attack      \n"
		"[s] + \x1\x2 Ranged Attack     \n"
		"[d] + \x1\x2 Interact          \n"
		"       [h] Use Health Potion   \n"
		"       [f] Inspect Mode        \n"
		"     [ESC] Pause/Exit Inspect  \n"
		"   [enter] Inspect/Exit Dialog \n"
		"      [F1] Show This Dialog    \n"
		"      [F2] Show Melee Weapon   \n"
		"      [F3] Show Ranged Weapon  \n"
		"      [F5] Reroll Color Palette",
		NULL
	);
#endif

	legend_menu = rend_dialog_create(
		"You are an adventurer, of some kind.\n"
		"You found a dungeon to explore.     \n"
		"Find as much gold as you can.       \n"
		"Good luck.                          ",
		NULL
	);

	next_floor_menu = rend_dialog_create(
		"You move deeper into the dungeon...", NULL
	);

	restart_menu = rend_dialog_create(
		"A new adventurer enters the depths.", TITLE_BG_FILE
	);

	/* Initialize the random function. */
	map_init_rand();

	/* Create the pause menu. */
	pause_menu = rend_dialog_create(
#ifdef VITABUILD
		"Game Paused\nPress \xFE to quit.", NULL
#elif __3DS__
		"Game Paused\nPress Start again to quit.", NULL
#else
		"Game Paused\nPress escape again to quit.", NULL
#endif
	);

	/* Generate the map. */
	cur_floor = config->map.floor_start;
	cur_map = map_gen_default(&(config->map), cur_floor);

	object_setmap(cur_map);

	info("Game initialized.\n", NULL);
	rend_text_append("Game started.");

	ticks = last_ticks = SDL_GetTicks();
	dt = 0;

	/* GC any initializer objects. */
	object_gc();

	/* Start playin some music! */
	audio_play_bgm();

	while (!quit) {
		quit = input_handle();
		quit = game_update(dt) || quit;
		quit = rend_handle_frame(cur_map) || quit;

		SDL_Delay(1000 / config->fps_limit);

		/* Generate next deltatime. */
		ticks = SDL_GetTicks();
		dt = (ticks - last_ticks) / 1000.0f;
		last_ticks = ticks;
	}

	map_free(cur_map);

	audio_free();

#ifdef NODEBUG
	rend_dialog_free(main_menu);
#endif
	rend_dialog_free(pause_menu);
	rend_close();

#ifdef NODEBUG
	printf("================================\n"
		   "Thank you for playing!\n");
#endif

#ifdef VITABUILD
	sceKernelExitProcess(0);
#endif
	return 0;
}

#define DEATH_MESSAGE	"You've succumbed to the dungeon.\n"
#define DEATH_FLOOR		"You reached floor "
#define DEATH_GOLD		",\nand collected \x1D"

#define DEATH_FMT		(DEATH_MESSAGE DEATH_FLOOR "%d" DEATH_GOLD "%d" ".")
#define DEATH_LEN		(strlen(DEATH_MESSAGE)\
		+ strlen(DEATH_FLOOR)\
		+ 10\
		+ strlen(DEATH_GOLD)\
		+ 10)

void
game_death_screen(object_t *player)
{
	char *msg;

	msg = malloc(DEATH_LEN);

	snprintf(msg, DEATH_LEN, DEATH_FMT,
		cur_floor, player->gold);

	death_menu = rend_dialog_create(
		msg, NULL
	);

	rend_set_dialog(death_menu);
	audio_set_bgm(AUD_DEATH);
}

int
game_update(float dt)
{
	object_t	*inspecter;
	SDL_Rect	 rend_dim;
	size_t 		 i;

	inspecter = object_first(OBJ_INSPECTER);

	if (rend_has_dialog(renderer)) {
		if (input_clicked(IN_CONTINUE)) {
#ifndef DEBUG
			if (renderer->cur_dialog == main_menu)
				rend_set_dialog(legend_menu);
			else if(renderer->cur_dialog == legend_menu)
				rend_set_dialog(control_menu);
			else
#endif
			if (renderer->cur_dialog == death_menu) {
				free(death_menu->message);
				rend_dialog_free(death_menu);
				map_free(cur_map);

				cur_floor = 0;
				cur_map = map_gen_default(&(config->map), cur_floor);
				object_setmap(cur_map);
				last_offset = (SDL_Point){ 0, 0 };

				renderer->cur_dialog = restart_menu;
				audio_stop_bgm();
				audio_set_bgm(AUD_MENU);
				audio_play_bgm();
			}
			else {
				audio_set_bgm(AUD_BGM);
				rend_set_dialog(NULL);
			}
		}
#ifdef VITABUILD
		/* On the Vita, clicking pause again should "toggle". */
		else if (input_clicked(IN_PAUSE)
				&& pause_menu == renderer->cur_dialog)
			rend_set_dialog(NULL);
		else if (input_clicked(IN_CANCEL)
				&& pause_menu == renderer->cur_dialog)
#else
		else if (input_clicked(IN_PAUSE)
				&& pause_menu == renderer->cur_dialog)
#endif
			return 1;
	}
	else if (inspecter)
		object_update(inspecter, dt);
	else {
		if (input_clicked(IN_PAUSE))
			rend_set_dialog(pause_menu);
		if (input_clicked(IN_HELP))
			rend_set_dialog(control_menu);
		if (input_clicked(IN_COLOR_ROLL))
			map_gen_colors(cur_map);

		for (i = 0; i < cur_map->objects_len; i++)
			object_update(cur_map->objects + i, dt);
	}

	/* Update the map rendering offset, to keep the player centered. */
	player = object_first(OBJ_PLAYER);
	if (NULL != player) {
		rend_dim = rend_get_tile_dimensions();

		if (rend_dim.w < cur_map->size.w) {
			cur_map->size.x = fminf(fmaxf(
				0,
				player->x - (rend_dim.w / 2)
			), cur_map->size.w - rend_dim.w);
		}

		if (rend_dim.h < cur_map->size.h) {
			cur_map->size.y = fminf(fmaxf(
				0,
				player->y - (rend_dim.h / 2)
			), cur_map->size.h - rend_dim.h);
		}

		if (last_offset.x == 0 && last_offset.y == 0)
			last_offset = (SDL_Point){ cur_map->size.x, cur_map->size.y };
		else {
			if (abs(last_offset.x - cur_map->size.x) > 1)
				cur_map->size.x = last_offset.x;

			if (abs(last_offset.y - cur_map->size.y) > 1)
				cur_map->size.y = last_offset.y;

			last_offset = (SDL_Point){ cur_map->size.x, cur_map->size.y };
		}

		/* If we're going to the next floor, trigger that. */
		if (OBJ_HAS(player, OBJ_PLAYER_NEXT_FLOOR)) {
			map_free(cur_map);
			cur_map = map_gen_default(&(config->map), ++cur_floor);
			object_setmap(cur_map);
			last_offset = (SDL_Point){ 0, 0 };

			renderer->cur_dialog = next_floor_menu;
		}
		else if (!rend_has_dialog(renderer) && OBJ_HAS(player, OBJ_PLAYER_DEAD))
			game_death_screen(player);
	}

	return 0;
}
