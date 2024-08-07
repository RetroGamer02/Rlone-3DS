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

#ifndef RLONE_RENDERER_DIALOG_H
#define RLONE_RENDERER_DIALOG_H

#include <SDL.h>

typedef struct {
	char 		*message;
	SDL_Texture *background;
	SDL_Texture	*rendered;
} dialog_t;

void		 rend_dialog_setrend(void *);
dialog_t	*rend_dialog_create(char *, const char *);
void		 rend_dialog_set_msg(dialog_t *, char *);
void		 rend_dialog_free(dialog_t *);
void		 rend_dialog_render(dialog_t *);

#endif
