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
#include <limits.h>

#include <SDL.h>

#include "map_gen.h"
#include "object.h"
#include "util.h"
#include "log.h"

/* The minimum difference between BG and other colors before an automatic
   reroll. */
#define MIN_DIFFERENCE	64

#define CALC_DIFF(m, r, g, b)	(\
  ( abs(m->bg_r - m->r)\
  + abs(m->bg_g - m->g)\
  + abs(m->bg_b - m->b) )\
	/ 3\
)

#define ROOM_AT(m,l,x,y)	((m)[((l) * (y)) + (x)])

static void			  _gen_base_row(map_t *, int, int, SDL_Point);
static room_t		**_gen_build_room_map(room_t *, size_t);
static SDL_Rect		  _gen_adjust_positions(room_t **, size_t);
static void			  _gen_build_hallways_and_doors(map_t *, room_t *);
static void			  _gen_planecast_hallway_x(map_t *, room_t *, int);
static void			  _gen_planecast_hallway_y(map_t *, room_t *, int);
static void			  _gen_place_columns(map_t *, room_t *);
static void			  _gen_place_objects(map_t *, room_t *);

map_t *
map_gen(room_t *rooms, size_t rooms_len, int floor)
{
	room_t	**room_map,
			 *cur_room;
	map_t	 *new;
	size_t	  x, y;

	/* Generate the room map to derive our real map from. */
	room_map = _gen_build_room_map(rooms, rooms_len);

	/* Put our real map together. */
	new = calloc(sizeof(map_t), 1);
	map_gen_colors(new);

	/* Get the size of our real map, to malloc the buffer.
	   This also fixes the positions in the rooms. */
	new->size = _gen_adjust_positions(room_map, rooms_len);
	new->buffer = malloc(sizeof(char) * new->size.w * new->size.h);
	memset(new->buffer, ' ', sizeof(char) * new->size.w * new->size.h);

	new->floor = floor;

	/* Add the GC. */
	map_new_object(new, OBJ_MANAGER, 0, 0);

	/* And the update ticker. */
	map_new_object(new, OBJ_TICKER, 0, 0);

	object_setmap(new);

	/* Build the rooms. */
	for (y = 0; y < rooms_len; y++) {
		for (x = 0; x < rooms_len; x++) {
			cur_room = ROOM_AT(room_map, rooms_len, x, y);
			if (NULL == cur_room)
				continue;

			map_gen_room(new, cur_room->pos, cur_room->size);
	
			if (cur_room->has_player) {
				/* Add the player object. */
				map_new_object(
					new, OBJ_PLAYER,
					cur_room->pos.x + (cur_room->size.x / 2),
					cur_room->pos.y + (cur_room->size.y / 2)
				);
			}

			if (cur_room->has_exit) {
				/* Add the player object. */
				map_new_object(
					new, OBJ_STAIRS,
					cur_room->pos.x + (cur_room->size.x / 2),
					cur_room->pos.y + (cur_room->size.y / 2)
				);
			}

			if (cur_room->has_starting_chest) {
				/* Add the player object. */
				map_new_object(
					new, OBJ_STARTER_CHEST,
					cur_room->pos.x + (cur_room->size.x / 2) - 1,
					cur_room->pos.y + (cur_room->size.y / 2) - 1
				);
			}

			if (cur_room->has_columns)
				_gen_place_columns(new, cur_room);
		}
	}

	/* Build the hallways and doors. */
	for (y = 0; y < rooms_len; y++) {
		for (x = 0; x < rooms_len; x++) {
			cur_room = ROOM_AT(room_map, rooms_len, x, y);
			if (NULL == cur_room)
				continue;

			_gen_build_hallways_and_doors(new, cur_room);
			_gen_place_objects(new, cur_room);
		}
	}

#ifdef DEBUG
	debug("Final map size: %d, %d\n", new->size.w, new->size.h);
#endif

	free(room_map);
	return new;
}

void
map_gen_colors(map_t *m)
{
	m->bg_r = RANGE(0, 64);
	m->bg_g = RANGE(0, 64);
	m->bg_b = RANGE(0, 64);

	do {
		m->f_r = RANGE(0, 128);
		m->f_g = RANGE(0, 128);
		m->f_b = RANGE(0, 128);
	} while (CALC_DIFF(m, f_r, f_g, f_b) <= MIN_DIFFERENCE);

	do {
		m->w_r = RANGE(0, 128);
		m->w_g = RANGE(0, 128);
		m->w_b = RANGE(0, 128);
	} while (CALC_DIFF(m, w_r, w_g, w_b) <= MIN_DIFFERENCE);
}

void
map_gen_room(map_t *m, SDL_Point off, SDL_Point size)
{
	int y, offset;

	/* Calculate the map offset. */
	offset = (off.y * m->size.w) + off.x;

	/* Generate the base room. */
	for (y = 0; y < size.y; y++)
		_gen_base_row(m, offset, y, size);
}

static void
_gen_base_row(map_t *m, int off, int y, SDL_Point size)
{
	int		i, idx;
	int		bufsize;

	bufsize = m->size.w * m->size.h;

	for (i = 0; i < size.x; i++) {
		idx = off + ((y * m->size.w) + i);
		/* TODO: Revisit this and see if theres a better way. */
		if (idx >= bufsize)
			continue;

		if (i == 0 || y == 0 || i == size.x - 1 || y == size.y - 1)
			m->buffer[idx] = MAP_WALL;
		else
			m->buffer[idx] = MAP_FLOOR;
	};	
}

/* My brain hurts. */
static room_t **
_gen_build_room_map(room_t *rooms, size_t rooms_len)
{
	room_t		**room_map;
	room_t		 *cur_room;
	int			  min_x_i = 0,
				  min_y_i = 0;
	size_t		  mi, i;

	/* First get the smallest indexes, for use in storage. */
	for (i = 0; i < rooms_len; i++) {
		cur_room = rooms + i;

		if (cur_room->pos.x < min_x_i)
			min_x_i = cur_room->pos.x;

		if (cur_room->pos.y < min_y_i)
			min_y_i = cur_room->pos.y;
	}

	/* Create a full map of where the rooms are relative to each other. */
	room_map = calloc(sizeof(room_t *), rooms_len * rooms_len);

	/* Abs the minimums to make the math more gooder. */
	min_x_i = abs(min_x_i);
	min_y_i = abs(min_y_i);

	/* Then actually store the values. */
	for (i = 0; i < rooms_len; i++) {
		cur_room = rooms + i;
		mi = (rooms_len * (min_y_i + cur_room->pos.y))
			+ (min_x_i + cur_room->pos.x);

		if (NULL != room_map[mi]) {
			warn("Duplicate room found at (%d,%d).\n",
				cur_room->pos.x, cur_room->pos.y);
			continue; /* Skip duplicate rooms. */
		}

		room_map[mi] = cur_room;
	}

	return room_map;
}

static SDL_Rect
_gen_adjust_positions(room_t **room_map, size_t rooms_len)
{
	room_t		*cur_room;
	SDL_Rect	 largest = { 0 },
				 current = { 0 };
	size_t		 x, y;

	for (x = 0; x < rooms_len; x++) {
		current = (SDL_Rect){ 0 };
		current.x = largest.w;

		for (y = 0; y < rooms_len; y++) {
			/* Find the largest y for the last row. */
			if (y > 0) {
			}

			cur_room = ROOM_AT(room_map, rooms_len, x, y);
			if (NULL == cur_room)
				continue;

			cur_room->pos.x = current.x;
			if (cur_room->size.x > current.w)
				current.w = cur_room->size.x;
		}

		largest.w += current.w;
	}

	/* DO IT AGAIN. */
	for (y = 0; y < rooms_len; y++) {
		current = (SDL_Rect){ 0 };
		current.y = largest.h;

		for (x = 0; x < rooms_len; x++) {
			cur_room = ROOM_AT(room_map, rooms_len, x, y);
			if (NULL == cur_room)
				continue;

			cur_room->pos.y = current.y;
			if (cur_room->size.y > current.h)
				current.h = cur_room->size.y;
		}

		largest.h += current.h;
	}

#ifdef DEBUG
	debug("Room Map:\n", NULL);
	for (y = 0; y < rooms_len; y++) {
		printf("\t");

		for (x = 0; x < rooms_len; x++) {
			cur_room = ROOM_AT(room_map, rooms_len, x, y);
			if (NULL == cur_room) {
				printf("(%2d,%2d)(%2d,%2d) ",
					0, 0, 0, 0
				);
			}
			else {
				printf("(%2d,%2d)(%2d,%2d) ",
					cur_room->pos.x, cur_room->pos.y,
					cur_room->size.x, cur_room->size.y
				);
			}
		}

		printf("\n");
	}
#endif

	return largest;
}

static void
_gen_build_hallways_and_doors(map_t *map, room_t *room)
{
	int	i;

	for (i = 0; i < 4; i++) {
		if (room->doors[i]) {
			switch (i) {
			case 0:
				_gen_planecast_hallway_x(map, room, -1);
				break;
			case 1:
				_gen_planecast_hallway_y(map, room, -1);
				break;
			case 2:
				_gen_planecast_hallway_x(map, room, 1);
				break;
			case 3:
				_gen_planecast_hallway_y(map, room, 1);
				break;
			}
		}
	}
}

static void
_gen_planecast_hallway_x(map_t *map, room_t *room, int dir)
{
	int 		x,
				y,
				start,
				end;
	char		found_room	= 0;
	SDL_Rect	oth_wall	= { 0 };
	/* For use with room gen. */
	SDL_Point	hall_pos	= { 0 },
				hall_size	= { 0 };

	start = room->pos.x;
	if (dir > 0)
		start += room->size.x;

	end = room->pos.y + room->size.y;

	for (x = start; x >= 0 && x < map->size.w; x += dir) {
		for (y = room->pos.y; y < end; y++) {
			if (x == start) {
				if (map_get_at(map, x, y) != MAP_WALL)
					return; /* This room already has a door here. */
			}
			else {
				if (map_get_at(map, x, y) == MAP_WALL) {
					if (!found_room) {
						oth_wall.x = x;
						oth_wall.y = y;
					}

					found_room = 1;
					oth_wall.h++;
				}

				if (found_room && map_get_at(map, x, y) != MAP_WALL)
					goto breakout_x;
			}
		}
	}

	if (!found_room)
		return;

breakout_x:
	hall_size.y = 3;
	hall_size.x = abs(start - oth_wall.x);
	hall_pos.x = MIN(start, oth_wall.x);
	hall_pos.y = MIN(room->pos.y, oth_wall.y)
		+ (MIN(room->size.y, oth_wall.h) / 2 - 1);

	/* Weird rogue door got generated for some reason. */
	if (hall_pos.y < room->pos.y)
		return;

#ifdef DEBUG
	debug("Generating H hall: (%d,%d)(%d) -> (%d,%d)(%d) = (%d,%d)(%d,%d)\n",
		start, room->pos.y, room->size.y,
		oth_wall.x, oth_wall.y, oth_wall.h,
		hall_pos.x, hall_pos.y,
		hall_size.x, hall_size.y
	);
#endif
	/* Generate the hallways themselves. */
	if (hall_size.x > 1) {
		hall_size.x++;
		map_gen_room(map, hall_pos, hall_size);
		hall_size.x--;
	}

	/* Generate the doors. */
	map_set_at(map, hall_pos.x, hall_pos.y + 1, MAP_FLOOR);
	map_set_at(map, hall_pos.x + hall_size.x, hall_pos.y + 1, MAP_FLOOR);

	if (hall_size.x > 3) {
		/* Add 2 door */
		map_new_object(map, OBJ_DOOR, hall_pos.x, hall_pos.y + 1);
		map_new_object(map, OBJ_DOOR, hall_pos.x + hall_size.x, hall_pos.y + 1);
	}
	else {
		/* Add 1 door */
		map_new_object(map, OBJ_DOOR, hall_pos.x + (hall_size.x / 2), hall_pos.y + 1);
	}
}

static void
_gen_planecast_hallway_y(map_t *map, room_t *room, int dir)
{
	int 		x,
				y,
				start,
				end;
	char		found_room	= 0;
	SDL_Rect	oth_wall	= { 0 };
	/* For use with room gen. */
	SDL_Point	hall_pos	= { 0 },
				hall_size	= { 0 };

	start = room->pos.y;
	if (dir > 0)
		start += room->size.y;

	end = room->pos.x + room->size.x;

	for (y = start; y >= 0 && y < map->size.h; y += dir) {
		for (x = room->pos.x; x < end; x++) {
			if (y == start) {
				if (map_get_at(map, x, y) != MAP_WALL)
					return; /* This room already has a door here. */
			}
			else {
				if (map_get_at(map, x, y) == MAP_WALL) {
					if (!found_room) {
						oth_wall.x = x;
						oth_wall.y = y;
					}

					found_room = 1;
					oth_wall.w++;
				}

				if (found_room && map_get_at(map, x, y) != MAP_WALL)
					goto breakout_y;
			}
		}
	}

	if (!found_room)
		return;

breakout_y:
	hall_size.x = 3;
	hall_size.y = abs(start - oth_wall.y);
	hall_pos.y = MIN(start, oth_wall.y);
	hall_pos.x = MIN(room->pos.x, oth_wall.x)
		+ (MIN(room->size.x, oth_wall.w) / 2 - 1);

	/* Weird rogue door got generated for some reason. */
	if (hall_pos.x < room->pos.x)
		return;

#ifdef DEBUG
	debug("Generating V hall: (%d,%d)(%d) -> (%d,%d)(%d) = (%d,%d)(%d,%d)\n",
		room->pos.x, start, room->size.x,
		oth_wall.x, oth_wall.y, oth_wall.w,
		hall_pos.x, hall_pos.y,
		hall_size.x, hall_size.y
	);
#endif
	/* Generate the hallways themselves. */
	if (hall_size.y > 1) {
		hall_size.y++;
		map_gen_room(map, hall_pos, hall_size);
		hall_size.y--;
	}

	/* Generate the doors. */
	map_set_at(map, hall_pos.x + 1, hall_pos.y, MAP_FLOOR);
	map_set_at(map, hall_pos.x + 1, hall_pos.y + hall_size.y, MAP_FLOOR);

	if (hall_size.y > 3) {
		map_new_object(map, OBJ_DOOR, hall_pos.x + 1, hall_pos.y);
		map_new_object(map, OBJ_DOOR, hall_pos.x + 1, hall_pos.y + hall_size.y);
	}
	else 
		map_new_object(map, OBJ_DOOR, hall_pos.x + 1, hall_pos.y + (hall_size.y / 2));
}

static void
_gen_place_columns(map_t *map, room_t *room)
{
	SDL_Point	placements;

	/* If size if even, and only adding 2 columns, skip instead. */
	if ((room->size.x % 2 == 0 && room->size.x > 14 && room->size.y <= 6)
	 || (room->size.y % 2 == 0 && room->size.y > 12 && room->size.x <= 7))
		return;

	if (room->size.x < 5 || room->size.y < 5)
		return; /* Stop creating columns if they may block doors. */

	placements = (SDL_Point){
		room->size.x / 4,
		room->size.y / 4
	};

	if (room->size.y <= 12)
		placements.y = room->size.y / 2;
	if (room->size.x <= 14)
		placements.x = room->size.x / 2;

	if (room->size.x > 14) {
		map_new_object(map,
			OBJ_COLUMN,
			room->pos.x + placements.x,
			room->pos.y + placements.y
		);

		map_new_object(map,
			OBJ_COLUMN,
			room->pos.x + room->size.x - placements.x - 1,
			room->pos.y + placements.y
		);

		if (room->size.y > 12) {
			map_new_object(map,
				OBJ_COLUMN,
				room->pos.x + placements.x,
				room->pos.y + room->size.y - placements.y - 1
			);

			map_new_object(map,
				OBJ_COLUMN,
				room->pos.x + room->size.x - placements.x - 1,
				room->pos.y + room->size.y - placements.y - 1
			);
		}
	}
	else {
		map_new_object(map,
			OBJ_COLUMN,
			room->pos.x + placements.x,
			room->pos.y + placements.y
		);

		map_new_object(map,
			OBJ_COLUMN,
			room->pos.x + placements.x,
			room->pos.y + room->size.y - placements.y - 1
		);
	}
}

static void
_gen_place_objects(map_t *map, room_t *room)
{
	SDL_Point	loc;
	int			i;

	for (i = 0; i < room->num_loot; i++) {
		do {
			loc.x = room->pos.x + 1 + ROLL(room->size.x - 1);
			loc.y = room->pos.y + 1 + ROLL(room->size.y - 1);
		} while(NULL != object_at(loc.x, loc.y));

		map_new_object(map, OBJ_LOOT, loc.x, loc.y);
	}

	for (i = 0; i < room->num_mobs; i++) {
		do {
			loc.x = room->pos.x + 1 + ROLL(room->size.x - 1);
			loc.y = room->pos.y + 1 + ROLL(room->size.y - 1);
		} while(NULL != object_at(loc.x, loc.y));

		map_new_object(map, OBJ_MOB, loc.x, loc.y);
	}
}
