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

#include "object_projectile_arrow.h"
#include "object_npc.h"

#include "renderer_entity.h"
#include "util.h"

void
object_projectile_arrow_init(object_t *o)
{
	o->ent = ENT_ARROW;

	if (o->type == OBJ_PROJECTILE_ARROW_PATHED) {
		o->type = OBJ_PROJECTILE_ARROW;
		OBJ_SET(o, OBJ_PROJECTILE_PATHED);
	}
}

void
object_projectile_arrow_update(object_t *o)
{
	object_t	*t;
	SDL_Point	 move = { 0 };

	if (OBJ_IS(o, OBJ_PROJECTILE_PATHED)) {
		move = object_npc_pathfind_to_point(o, o->dmg, o->dmg_max);

		if (o->x + move.x == o->dmg && o->y + move.y == o->dmg_max) {
			OBJ_REM(o, OBJ_PROJECTILE_PATHED);

			switch (move.x) {
			case -1:
				OBJ_SET(o, OBJ_PROJECTILE_LEFT);
				break;
			case 1:
				OBJ_SET(o, OBJ_PROJECTILE_RIGHT);
				break;
			default:
				break;
			}

			switch (move.y) {
			case -1:
				OBJ_SET(o, OBJ_PROJECTILE_UP);
				break;
			case 1:
				OBJ_SET(o, OBJ_PROJECTILE_DOWN);
				break;
			default:
				break;
			}
		}
	}
	else {
		if (OBJ_HAS(o, OBJ_PROJECTILE_UP))
			move.y--;
		if (OBJ_HAS(o, OBJ_PROJECTILE_DOWN))
			move.y++;
		if (OBJ_HAS(o, OBJ_PROJECTILE_LEFT))
			move.x--;
		if (OBJ_HAS(o, OBJ_PROJECTILE_RIGHT))
			move.x++;
	}

	if (move.x == 0 && move.y == 0) {
		o->enabled = 0;
		return;
	}

	o->y += move.y;
	o->x += move.x;

	if (!object_collision_at(o->x, o->y))
		return;

	/* Assume whatever created us has set the appropriate values. */
	t = object_at(o->x, o->y);
	o->enabled = 0;

	if (t != o && NULL != t) {
		object_hit(
			t,
			RANGE(o->hp, o->hp_max),
			RANGE(o->ac, o->ac_max)
		);
	}
}
