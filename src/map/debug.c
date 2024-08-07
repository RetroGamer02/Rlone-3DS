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

#include "map_debug.h"
#include "map.h"

#include "object.h"

#ifdef __3DS__
#include <stdlib.h>
#endif

void	_gen_debug_row(map_t *, int);

map_t *
map_gen_debug(int w, int h)
{
	object_t	*p;
	map_t		*new;

	new = calloc(sizeof(map_t), 1);
	map_gen_colors(new);

	/* Setup the actual default map. */
	new->size = (SDL_Rect){
		0, 0, w, h
	};

	new->buffer = malloc(sizeof(char) * w * h);

	map_gen_room(new, (SDL_Point){ 0, 0 }, (SDL_Point){ w, h });

	/* Add a player entity. */
	map_new_object(new, OBJ_PLAYER, w / 2, h / 2);

	/* Add a player entity. */
	object_setmap(new);
	map_new_object(new, OBJ_NPC_GOBLIN, w / 2 + 1, h / 2 + 1);

	p = object_first(OBJ_PLAYER);
	p->gold = 100;

	return new;
}

void
_gen_debug_row(map_t *m, int y)
{
	int	i, idx;

	for (i = 0; i < m->size.w; i++) {
		idx = (y * m->size.w) + i;

		if (i == 0 || y == 0 || i == m->size.w - 1 || y == m->size.h - 1)
			m->buffer[idx] = MAP_WALL;
		else
			m->buffer[idx] = MAP_FLOOR;
	};	
}
