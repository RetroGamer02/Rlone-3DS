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

#include "object_zombie.h"
#include "object_npc.h"

#include "object_slash.h"
#include "object_door.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "util.h"

static dialog_t	*inspect;
static dialog_t	*inspect_dead;

void
object_zombie_init(object_t *o)
{
	int	floor = object_get_floor();

	object_npc_init(o);
	o->ent = ENT_ZOMBIE;

	o->gold = ROLL(20 * floor);

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"The fate of a previous adventurer.", NULL
		);
	}
	else if (NULL == inspect_dead) {
		inspect_dead = rend_dialog_create(
			"The remains of a poor soul.", NULL
		);
	}

	OBJ_SET(o, OBJ_NPC_BEHAVIOR_AGGRESSIVE);
	OBJ_SET(o, OBJ_NPC_WANDERS);

	/* Roll our zombie! */
	o->hp = o->hp_max = RANGE(10 * floor, 40 * floor);
	o->ac = 0;
	o->ac_max = RANGE(1 * floor, 7 * floor);

	o->dmg = o->ap = 0;
	o->dmg_max = RANGE(1 * floor, 7 * floor) + 1;
	o->ap_max = RANGE(1 * floor, 5 * floor) + 1;

	o->inspect = inspect;
}

void
object_zombie_update(object_t *o)
{
	object_t	*target;
	SDL_Point	 raypos;
	SDL_Point	 move;
	float		 dist;

	if (OBJ_IS(o, OBJ_NPC_DEAD))
		return;

	target = object_first(OBJ_PLAYER);
	dist = object_npc_distance(o, target);
	raypos = object_npc_raycast_to(o, target->x, target->y);

	if (dist <= 8.0f && raypos.x == target->x && raypos.y == target->y) {
		OBJ_SET(o, OBJ_NPC_IDENTIFIED_PLAYER);
		o->ammo = 0;
	}
	else if(o->ammo++ > 10)
		OBJ_REM(o, OBJ_NPC_IDENTIFIED_PLAYER);

	if (dist <= 1.0f) {
		object_slash_create(
			target->x, target->y,
			o->dmg, o->dmg_max,
			o->ap, o->ap_max
		);
	}
	else {
		move = object_npc_update(o);

		if (OBJ_HAS(o, OBJ_NPC_IDENTIFIED_PLAYER)) {
			target = object_at(o->x + move.x, o->y + move.y);

			/* Attack whatever may be in the way. */
			if (NULL != target && target->type == OBJ_DOOR
			 && OBJ_HAS(target, OBJ_DOOR_CLOSED)) {
				object_slash_create(
					target->x, target->y,
					o->dmg, o->dmg_max,
					o->ap, o->ap_max
				);
			}
		}
	}
}

void
object_zombie_killed(object_t *o)
{
	OBJ_REM(o, OBJ_IS_COLLIDABLE);
	OBJ_SET(o, OBJ_NPC_DEAD);
	o->ent = ENT_ZOMBIE_DEAD;
	o->inspect = inspect_dead;

	object_drop(o);
}
