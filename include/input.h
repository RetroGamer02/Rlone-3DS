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

#ifndef RLONE_INPUT_H
#define RLONE_INPUT_H

#include <SDL.h>

enum input {
	IN_PAUSE,
	IN_CONTINUE,
#ifdef VITABUILD
	/* For Vita menus, as there's a standard to abide by. */
	IN_CANCEL,
#endif
	IN_COLOR_ROLL,
	IN_MOVE_UP,
	IN_MOVE_DOWN,
	IN_MOVE_LEFT,
	IN_MOVE_RIGHT,
	IN_DOOR,
	IN_INSPECT,
	IN_INTERACT,
	IN_HELP,
	IN_INSPECT_SWORD,
	IN_INSPECT_BOW,
	IN_SLASH,
	IN_SHOOT,
	IN_HEAL
};

/* Informs if there are any input changes this frame. */
int	input_has_change();

/* Triggers for 1 frame after a key goes from up to down. */
int	input_clicked(enum input);

/* Triggers if the key is currently down. */
int	input_is_down(enum input);

/* Triggers if the key is currently up. */
int	input_is_up(enum input);

/* Handler function for the game loop. */
int	input_handle();

#endif
