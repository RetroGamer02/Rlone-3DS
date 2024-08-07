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

#ifndef RLONE_RENDERER_H
#define RLONE_RENDERER_H

#include <SDL.h>

#include "renderer_dialog.h"

#include "config.h"
#include "map.h"

typedef struct {
	cfg_t		*cfg;

	SDL_Window		*win;
	SDL_Renderer	*backbuffer;

	SDL_Texture		*font;
	int				 font_w, font_h;

	dialog_t	*cur_dialog;
	int			 supports_targets;
} rend_t;

#define rend_has_dialog(r)	(NULL != r->cur_dialog)

rend_t	*rend_init(cfg_t *);
void	 rend_set_dialog(dialog_t *);
int		 rend_handle_frame(map_t *);
void	 rend_close();
SDL_Rect rend_get_tile_dimensions();
void	 rend_text_append(const char *fmt, ...);

#endif
