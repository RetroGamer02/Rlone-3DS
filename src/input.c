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

#include <stddef.h>

#include "input.h"

#ifdef VITABUILD
#include <psp2/ctrl.h>
static SceCtrlData	ctrl;
#endif

#ifdef __3DS__
#include <3ds.h>
#endif

typedef struct {
#ifdef VITABUILD
	int			sym;
	/* If set, will ignore any movement bit flags when checking. */
	int			directional;
#else
	SDL_Keycode	sym;
#endif
	int			status;
	int			clicked;
} imap_t;

#if defined (VITABUILD) || defined (__3DS__)
void	_update_imap(int, int);
#else
void	_update_imap(int, SDL_Keysym);
#endif
void	_update_clicked();

static imap_t input_map[] = {
#ifdef VITABUILD
	[IN_PAUSE]      	= { SCE_CTRL_START,                        0, 0, 0 },
	[IN_CONTINUE]   	= { SCE_CTRL_CIRCLE,                       0, 0, 0 },
	[IN_CANCEL]			= { SCE_CTRL_CROSS,                        0, 0, 0 },
	[IN_COLOR_ROLL] 	= { SCE_CTRL_RTRIGGER | SCE_CTRL_LTRIGGER, 0, 0, 0 },
	[IN_MOVE_UP]    	= { SCE_CTRL_UP,                           2, 0, 0 },
	[IN_MOVE_DOWN]  	= { SCE_CTRL_DOWN,                         2, 0, 0 },
	[IN_MOVE_LEFT]  	= { SCE_CTRL_LEFT,                         2, 0, 0 },
	[IN_MOVE_RIGHT] 	= { SCE_CTRL_RIGHT,                        2, 0, 0 },
	[IN_INSPECT]		= { SCE_CTRL_SELECT,                       0, 0, 0 },
	[IN_INTERACT]		= { SCE_CTRL_CIRCLE,                       1, 0, 0 },
	[IN_HELP]			= { SCE_CTRL_LTRIGGER | SCE_CTRL_CIRCLE,   0, 0, 0 },
	[IN_INSPECT_SWORD]	= { SCE_CTRL_LTRIGGER | SCE_CTRL_CROSS,    0, 0, 0 },
	[IN_INSPECT_BOW]	= { SCE_CTRL_LTRIGGER | SCE_CTRL_SQUARE,   0, 0, 0 },
	[IN_SLASH]			= { SCE_CTRL_CROSS,                        1, 0, 0 },
	[IN_SHOOT]			= { SCE_CTRL_SQUARE,                       1, 0, 0 },
	[IN_HEAL]			= { SCE_CTRL_TRIANGLE,                     0, 0, 0 },
#else
	[IN_PAUSE]      	= { SDLK_ESCAPE, 0, 0 },
	[IN_CONTINUE]   	= { SDLK_RETURN, 0, 0 },
	[IN_COLOR_ROLL] 	= { SDLK_F5,     0, 0 },
	[IN_MOVE_UP]    	= { SDLK_UP,     0, 0 },
	[IN_MOVE_DOWN]  	= { SDLK_DOWN,   0, 0 },
	[IN_MOVE_LEFT]  	= { SDLK_LEFT,   0, 0 },
	[IN_MOVE_RIGHT] 	= { SDLK_RIGHT,  0, 0 },
	[IN_INSPECT]		= { SDLK_f,      0, 0 },
	[IN_INTERACT]		= { SDLK_d,      0, 0 },
	[IN_HELP]			= { SDLK_F1,     0, 0 },
	[IN_INSPECT_SWORD]	= { SDLK_F2,     0, 0 },
	[IN_INSPECT_BOW]	= { SDLK_F3,     0, 0 },
	[IN_SLASH]			= { SDLK_a,      0, 0 },
	[IN_SHOOT]			= { SDLK_s,      0, 0 },
	[IN_HEAL]			= { SDLK_h,      0, 0 },
#endif
};
static int	input_map_len = (sizeof(input_map) / sizeof(input_map[0]));

int
input_has_change()
{
	int i;

	/* See if anything has clicked, as that indicates a change state. */
	for (i = 0; i < input_map_len; i++) {
		if (input_map[i].clicked == input_map[i].status)
			return 1;
	}

	return 0;
}

int
input_clicked(enum input i)
{
	return input_map[i].status && input_map[i].clicked;
}

int
input_is_down(enum input i)
{
	return input_map[i].status;
}

int
input_is_up(enum input i)
{
	return !input_map[i].status;
}

int
input_handle()
{
	SDL_Event	ev;
#ifdef VITABUILD
	int			i;
	int			btns;
#endif

	/* Must update the clicked flag first. */
	_update_clicked();

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			return 1;
#if !defined (VITABUILD) && !defined (__3DS__)
		case SDL_KEYDOWN:
			_update_imap(1, ev.key.keysym);
			break;
		case SDL_KEYUP:
			_update_imap(0, ev.key.keysym);
			break;
#endif
		}
	}

#ifdef __3DS__
        //hidScanInput();

        u32 kDown = hidKeysDown();
		u32 kUp = hidKeysUp();

		switch (kDown) {
			case KEY_SELECT: 
			_update_imap(1, 13);
			break;
			case KEY_START: 
			_update_imap(1, 27);
			break;
			case KEY_DUP:
			_update_imap(1, 1073741906);
			break;
			case KEY_DDOWN:
			_update_imap(1, 1073741905);
			break;
			case KEY_DLEFT:
			_update_imap(1, 1073741904);
			break;
			case KEY_DRIGHT:
			_update_imap(1, 1073741903);
			break;
			case KEY_R:
			_update_imap(1, 102);
			break;
			case KEY_L:
			_update_imap(1, 1073741882);
			break;
			case KEY_A:
			_update_imap(1, 100);
			break;
			case KEY_B:
			_update_imap(1, 97);
			break;
			case KEY_X:
			_update_imap(1, 115);
			break;
			case KEY_Y:
			_update_imap(1, 104);
			break;
		}

		switch (kUp) {
			case KEY_SELECT: 
			_update_imap(0, 13);
			break;
			case KEY_START: 
			_update_imap(0, 27);
			break;
			case KEY_DUP:
			_update_imap(0, 1073741906);
			break;
			case KEY_DDOWN:
			_update_imap(0, 1073741905);
			break;
			case KEY_DLEFT:
			_update_imap(0, 1073741904);
			break;
			case KEY_DRIGHT:
			_update_imap(0, 1073741903);
			break;
			case KEY_R:
			_update_imap(0, 102);
			break;
			case KEY_L:
			_update_imap(0, 1073741882);
			break;
			case KEY_A:
			_update_imap(0, 100);
			break;
			case KEY_B:
			_update_imap(0, 97);
			break;
			case KEY_X:
			_update_imap(0, 115);
			break;
			case KEY_Y:
			_update_imap(0, 104);
			break;
		}
#endif

#ifdef VITABUILD
	/* Utilize the PS Vita's own input system. */
	sceCtrlPeekBufferPositive(0, &ctrl, 1);

	for (i = 0; i < input_map_len; i++) {
		switch (input_map[i].directional) {
		case 2:
			/* This input is a direction, so only check for itself. */
			input_map[i].status = ctrl.buttons & input_map[i].sym;
			break;
		case 1:
			/* This input is directional, so don't worry about movement keys */
			btns = ctrl.buttons & ~(
				input_map[IN_MOVE_UP].sym | input_map[IN_MOVE_DOWN].sym |
				input_map[IN_MOVE_LEFT].sym | input_map[IN_MOVE_RIGHT].sym
			);

			input_map[i].status = btns == input_map[i].sym;
			break;
		default:
			input_map[i].status = ctrl.buttons == input_map[i].sym;
			break;
		}
	}
#endif

	return 0;
}

void
#if defined (VITABUILD) || defined (__3DS__)
_update_imap(int active, int sym)
#else
_update_imap(int active, SDL_Keysym sym)
#endif
{
	int	i = 0;

	for (i = 0; i < input_map_len; i++) {
#if defined (VITABUILD) || defined (__3DS__)
		if (input_map[i].sym == sym)
#else
		if (input_map[i].sym == sym.sym)
#endif
			input_map[i].status = active;
	}
}

void
_update_clicked()
{
	int	i = 0;

	for (i = 0; i < input_map_len; i++) {
		switch (input_map[i].status) {
		case 1:
			input_map[i].clicked = 0;
			break;
		case 0:
		default:
			/* Keep clicked enabled when the key is up, since the check
			   requires both to be active. */
			input_map[i].clicked = 1;
			break;
		}
	}
}
