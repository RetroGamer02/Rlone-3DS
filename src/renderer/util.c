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

#include <SDL.h>

#include "renderer_util.h"
#include "renderer.h"

static rend_t	*renderer;

void
rend_util_setrend(void *r)
{
	renderer = (rend_t *)r;
}

SDL_Rect
rend_get_char(char v)
{
	SDL_Rect final;

	final.w = renderer->cfg->font_x;
	final.h = renderer->cfg->font_y;

	/* Calculate where this character is on our map. */
	final.x = (final.w * v) % renderer->font_w;
	final.y = final.h * (v / (renderer->font_w / final.w));

	return final;
}
