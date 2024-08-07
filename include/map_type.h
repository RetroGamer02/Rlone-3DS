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

#ifndef RLONE_MAP_TYPE_H
#define RLONE_MAP_TYPE_H

#include <SDL.h>

#include "object.h"

#define MAP_FLOOR	('.')
#define MAP_WALL	('\x1F')

typedef struct {
	/* BG Color */
	int bg_r, bg_g, bg_b;

	/* Floor Color */
	int f_r, f_g, f_b;

	/* Wall color */
	int w_r, w_g, w_b;

	/* Map size */
	SDL_Rect	 size;

	/* Map data */
	char		*buffer;

	/* Map entities. */
	size_t		 objects_len;
	object_t	*objects;

	/* The current floor. */
	int			 floor;
} map_t;

#endif
