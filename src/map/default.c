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

#include "map_default.h"
#include "map_gen.h"

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "util.h"
#include "log.h"

struct map_default {
	room_t				*rooms;
	size_t				 rooms_len;
	size_t				 branches_len;
	room_t				*start_room,
						*end_room;

	float				 floor;
};

typedef struct map_default map_default_t;

static void		 _gen_free(map_default_t *);
static void		 _gen_rooms(cfg_map_t *, map_default_t *, SDL_Point, size_t, size_t);
static room_t	*_room_at(map_default_t *, SDL_Point);

/* Default map generation code. Used for normal game operation. */
map_t *
map_gen_default(cfg_map_t *cfg, int floor)
{
	map_default_t	*gen;
	map_t			*new;
	SDL_Point		 start = { 0, 0 };
	room_t			*base_room = NULL;
	size_t			 i,
					 offset = 0,
					 base_rooms_len,
					 gen_rooms_len = 0;

#ifdef DEBUG
	debug("Generating floor %d...\n", floor);
#endif

	gen = malloc(sizeof(map_default_t));

	/* Generate the number of rooms to be generated for the correct path. */
	base_rooms_len = cfg->room_num + ((float)floor * cfg->floor_mul);
	gen->rooms_len = base_rooms_len;

	/* Same but branches. */
	gen->branches_len = cfg->branch_num + ((float)floor * cfg->floor_mul);

	gen->floor = (float)floor * cfg->floor_mul;

	/* Add the rooms to be used by the branches. */
	for (i = 0; i < gen->branches_len; i++)
		gen->rooms_len += base_rooms_len / (2 * (i + 1));

#ifdef DEBUG
	debug("Rooms: %d, Branches: %d\n", gen->rooms_len, gen->branches_len);
#endif

	/* Allocate our main room buffer. */
	gen->rooms = calloc(sizeof(room_t), gen->rooms_len);
	gen->rooms[0].has_player = 1;
	if (cfg->floor_start == floor)
		gen->rooms[0].has_starting_chest = 1;

	/* Generate our rooms. */
	for (i = 0; i < gen->branches_len + 1 && offset < gen->rooms_len; i++) {
		_gen_rooms(cfg, gen, start, offset, base_rooms_len);
		gen_rooms_len += base_rooms_len;

		/* Set the new start based off a randomly generated room in the last
		   pass. */
		base_room = &gen->rooms[
			(int)(rand() / (float)RAND_MAX) * gen_rooms_len
		];

		start = (SDL_Point){
			base_room->pos.x,
			base_room->pos.y
		};

		offset += base_rooms_len;
		base_rooms_len = MAX(base_rooms_len / 2, 1);
	}

	gen->end_room->has_exit = 1;
	new = map_gen(gen->rooms, gen->rooms_len, floor);

	info("Floor %d generated.\n", floor);
	_gen_free(gen);
	return new;
}

static void
_gen_rooms(cfg_map_t *cfg, map_default_t *gen, SDL_Point start, size_t off, size_t len)
{
	room_t		*cur_room,
				*oth_room,
				*ex_room;
	size_t		 i, dir;

#ifdef DEBUG
	debug("Generating rooms %d thru %d...\n", off, off + len - 1);
#endif

	/* If this is the first run, it's the correct path. Set start and end. */
	if (0 == off) {
		gen->start_room = gen->rooms;
		gen->end_room = gen->rooms + len - 1;
	}

	for (i = off; i < (off + len); i++) {
		cur_room = gen->rooms + i;

		/* Only roll if not the initial room. */
		if (0 != i) {
			/* The room we are branching from. */
			oth_room = _room_at(gen, start);

			/* Gather the directional roll, and update the position. */
			dir = ROLL(4);
			switch (dir) {
			case 0: /* Left */
				start.x--;
				break;
			case 1: /* Up */
				start.y--;
				break;
			case 2: /* Right */
				start.x++;
				break;
			case 3: /* Down */
				start.y++;
				break;
			default:
				fatal("If this happens, I'm sorry: %d\n", dir);
			}

			/* Open the root door. */
			oth_room->doors[dir] = 1;

			/* A possible existing room at our coordinates. */
			ex_room = _room_at(gen, start);
			if (NULL != ex_room) {
#ifdef DEBUG
				debug("Branched into existing room at (%d,%d).\n",
					start.x, start.y);
#endif
				ex_room->doors[(dir + 2) % 4] = 1;

				/* Decrement i as the room was not consumed. */
				i--;
				continue;
			}
			else
				cur_room->doors[(dir + 2) % 4] = 1;
		}

		cur_room->pos = start;
		cur_room->size = (SDL_Point){
			RANGE(cfg->room_min.x, cfg->room_max.x),
			RANGE(cfg->room_min.y, cfg->room_max.y),
		};

		/* Determine if columns are needed. */
		if ((cur_room->size.x > 14 && cur_room->size.y > 2)
		 || (cur_room->size.y > 12 && cur_room->size.x > 2))
			cur_room->has_columns = 1;

#define REL_ROOM_SIZE(D,M)	(MIN(M * MAX(1, gen->floor), (cur_room->size.x * cur_room->size.y) / D))
		/* Roll how many mobs and loot there is. */
		if (off != 0 || (off == 0 && i != off)) {
			cur_room->num_loot = RANGE(0, REL_ROOM_SIZE(4, 2));
			cur_room->num_mobs = RANGE(0, REL_ROOM_SIZE(4, 2));
		}
	}
}

static void
_gen_free(map_default_t *gen)
{
	free(gen->rooms);
	free(gen);
}

static room_t *
_room_at(map_default_t *gen, SDL_Point loc)
{
	size_t i;

	for (i = 0; i < gen->rooms_len; i++) {
		if (gen->rooms[i].pos.x == loc.x
		 && gen->rooms[i].pos.y == loc.y)
			return gen->rooms + i;
	}

	return NULL;
}
