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

#ifndef RLONE_MAP_GEN_H
#define RLONE_MAP_GEN_H

#include "config.h"
#include "map.h"

typedef struct {
	SDL_Point	pos,
				size;
	int			doors[4];
	int			has_player;
	int			has_exit;
	int			has_starting_chest;
	int			has_columns;
	int			num_loot;
	int			num_mobs;
} room_t;

map_t	*map_gen(room_t *, size_t, int);
void	 map_gen_room(map_t *, SDL_Point, SDL_Point);
void	 map_gen_colors(map_t *);

#endif
