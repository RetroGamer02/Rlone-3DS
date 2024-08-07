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
#include <math.h>

#include "object_npc.h"

#include "renderer_entity.h"
#include "util.h"

#define PI		3.14159262
#define DEGRAD	(180 / PI)
#define RAD360	(360 / DEGRAD)

void
object_npc_init(object_t *o)
{
	o->ent = ENT_NOTFOUND;

	OBJ_SET(o, OBJ_IS_COLLIDABLE);
}

SDL_Point
object_npc_update(object_t *o)
{
	object_t	*player;
	SDL_Point	 move = { 0 };

	if (OBJ_IS(o, OBJ_NPC_DEAD))
		return move;

	if (OBJ_HAS(o, OBJ_NPC_IDENTIFIED_PLAYER)) {
		player = object_first(OBJ_PLAYER);

		if (OBJ_HAS(o, OBJ_NPC_BEHAVIOR_PASSIVE)) {
			/* Just keep wandering, or standing still. */
			if (OBJ_HAS(o, OBJ_NPC_WANDERS)) {
				move.x = RANGE(-2, 2);
				move.y = RANGE(-2, 2);
			}
		}
		else if (OBJ_HAS(o, OBJ_NPC_BEHAVIOR_SCARED)) {
			move = object_npc_pathfind_to(o, player);
			move.x = -1 * move.x;
			move.y = -1 * move.y;
		}
		else if (OBJ_HAS(o, OBJ_NPC_BEHAVIOR_AGGRESSIVE))
			move = object_npc_pathfind_to(o, player);
	}
	else if (OBJ_HAS(o, OBJ_NPC_WANDERS)) {
		move.x = RANGE(-2, 2);
		move.y = RANGE(-2, 2);
	}

	if (!object_collision_at(o->x + move.x, o->y + move.y)) {
		OBJ_REM(o, OBJ_NPC_STUCK);
		o->x += move.x;
		o->y += move.y;

		move = (SDL_Point){ 0 };
	}
	else if (!object_collision_at(o->x + move.x, o->y)) {
		OBJ_REM(o, OBJ_NPC_STUCK);
		o->x += move.x;

		move.y = 0;
	}
	else if (!object_collision_at(o->x, o->y + move.y)) {
		OBJ_REM(o, OBJ_NPC_STUCK);
		o->y += move.y;

		move.x = 0;
	}
	else if (object_is_wall_at(o->x + move.x, o->y + move.y)) {
		OBJ_SET(o, OBJ_NPC_STUCK);

		OBJ_REM(o, OBJ_NPC_HAS_DOOR_X);
		OBJ_REM(o, OBJ_NPC_HAS_DOOR_Y);
	}

	return move;
}

static float
_npc_get_angle(int x1, int y1, int x2, int y2)
{
	float a, b, omega;

	a = (float)x2 - x1;
	b = (float)y2 - y1;

	omega = atanf(b / a);

	if (a < 0)
		return (180 / DEGRAD) - omega;
	else if (b <= 0)
		return fabsf(omega);
	else
		return (360 / DEGRAD) - omega;
}

static SDL_Point
_npc_pathfind_step(int x1, int y1, int x2, int y2)
{
	float 		angle = _npc_get_angle(x1, y1, x2, y2);
	float		m_x, m_y;
	SDL_Point	final,
				mods = { 3, -3 };

	/* Dumb and bad hacky pathfinding because I don't have the time or
	   patience right now. */

	m_x = cos(angle);
	m_y = sin(angle);

	final = (SDL_Point){
		fminf(1, fmaxf(-1, (float)mods.x * m_x)),
		fminf(1, fmaxf(-1, (float)mods.y * m_y)),
	};

	return final;
}

SDL_Point
object_npc_raycast_to(object_t *from, int d_x, int d_y)
{
	SDL_Point	step;
	SDL_Point	raypos;

	raypos.x = from->x;
	raypos.y = from->y;

	/* Sanity check. */
	if (from->x == d_x && from->y == d_y)
		return raypos;

	do {
		step = _npc_pathfind_step(raypos.x, raypos.y, d_x, d_y);

		/* Apply the step. */
		raypos.x += step.x;
		raypos.y += step.y;

		if (raypos.x == d_x && raypos.y == d_y)
			break;

	} while (!object_collision_at(raypos.x, raypos.y));

	return raypos;
}

float
object_npc_distance(object_t *from, object_t *to)
{
	return sqrt(
		pow(abs(to->x - from->x), 2)
	  + pow(abs(to->y - from->y), 2)
	);
}

static int
_npc_correctly_facing(object_t *from, object_t *to, object_t *door, int alt)
{
	int diff_x, diff_y;

	diff_x = abs(from->x - to->x);
	diff_y = abs(from->y - to->y);

	if (diff_x == 0 || diff_y == 0) {
		OBJ_REM(from, OBJ_NPC_HAS_DOOR_X);
		OBJ_REM(from, OBJ_NPC_HAS_DOOR_Y);
	}
	else if (!OBJ_HAS(from, OBJ_NPC_STUCK)) {
		if (OBJ_HAS(from, OBJ_NPC_HAS_DOOR_X)) {
			return	(from->x < door->x && from->x < to->x) ||
					(from->x > door->x && from->x > to->x);
		}
		else if (OBJ_HAS(from, OBJ_NPC_HAS_DOOR_Y)) {
			return	(from->y < door->y && from->y < to->y) ||
					(from->y > door->y && from->y > to->y);
		}
	}

	if ((!alt && diff_x < diff_y) || (alt && diff_y < diff_x)) {
		return	(from->y < door->y && from->y < to->y) ||
				(from->y > door->y && from->y > to->y);
	}
	else {
		return	(from->x < door->x && from->x < to->x) ||
				(from->x > door->x && from->x > to->x);
	}
}

static int
_npc_can_see(object_t *from, object_t *to)
{
	SDL_Point raycast;

	raycast = object_npc_raycast_to(from, to->x, to->y);
	return raycast.x == to->x && raycast.y == to->y;
}

static object_t *
_npc_nearest_door(object_t *from, object_t *to, int alt)
{
	object_t	*near_door,
				*cur_door;
	float		 near_dist = INT_MAX,
				 cur_dist;

	object_iterate_restart();

	near_door = NULL;
	while (NULL != (cur_door = object_iterate(OBJ_DOOR))) {
		cur_dist = object_npc_distance(from, cur_door);

		if (_npc_correctly_facing(from, to, cur_door, alt) && (
			NULL == near_door ||
			cur_dist < near_dist
		)) {
			near_dist = cur_dist;
			near_door = cur_door;
		}
	}

	return near_door;
}

SDL_Point
object_npc_pathfind_to(object_t *from, object_t *to)
{
	object_t	*near_door;

	if (!_npc_can_see(from, to)) {
		/* The destination is not in view, find the nearest door. */
		near_door = _npc_nearest_door(from, to, OBJ_HAS(from, OBJ_NPC_STUCK));

		if (NULL != near_door) {
			/* Calculate which direction the door is, for the flags. */
			if (abs(from->x - to->x) <= abs(from->y - to->y))
				OBJ_SET(from, OBJ_NPC_HAS_DOOR_Y);
			else
				OBJ_SET(from, OBJ_NPC_HAS_DOOR_X);

			return _npc_pathfind_step(from->x, from->y, near_door->x, near_door->y);
		}
		else {
			OBJ_REM(from, OBJ_NPC_HAS_DOOR_X);
			OBJ_REM(from, OBJ_NPC_HAS_DOOR_Y);

			if (OBJ_HAS(from, OBJ_NPC_STUCK)) {
				/* Try again without the stuck flag. */
				OBJ_REM(from, OBJ_NPC_STUCK);
				return object_npc_pathfind_to(from, to);
			}
		}
	}

	return _npc_pathfind_step(from->x, from->y, to->x, to->y);
}

SDL_Point
object_npc_pathfind_to_point(object_t *from, int to_x, int to_y)
{
	return _npc_pathfind_step(from->x, from->y, to_x, to_y);
}
