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

#include "object_goblin.h"
#include "object_npc.h"

#include "object_door.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "util.h"

static dialog_t	*inspect;

void
object_goblin_init(object_t *o)
{
	int	floor = object_get_floor();

	object_npc_init(o);
	o->ent = ENT_GOBLIN;

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A mischievious goblin.", NULL
		);
	}

	OBJ_SET(o, OBJ_NPC_BEHAVIOR_AGGRESSIVE);
	OBJ_SET(o, OBJ_NPC_WANDERS);

	/* Roll our goblin! */
	o->hp = o->hp_max = RANGE(5 * floor, 20 * floor);
	o->ac = 0;
	o->ac_max = RANGE(3 * floor, 6 * floor);

	o->inspect = inspect;

	o->drops[0] = OBJ_GOLD;
	o->drops_len = 1;
}

void
object_goblin_update(object_t *o)
{
	object_t	*target;
	SDL_Point	 raypos;
	SDL_Point	 move;
	float		 dist;
	int			 gold_stolen;
	int			 floor = object_get_floor();

	if (OBJ_IS(o, OBJ_NPC_DEAD))
		return;

	target = object_first(OBJ_PLAYER);
	dist = object_npc_distance(o, target);
	raypos = object_npc_raycast_to(o, target->x, target->y);

	if (dist <= 16.0f && raypos.x == target->x && raypos.y == target->y) {
		OBJ_SET(o, OBJ_NPC_IDENTIFIED_PLAYER);
		o->ammo = 0;
	}
	else if(o->ammo++ > 20)
		OBJ_REM(o, OBJ_NPC_IDENTIFIED_PLAYER);

	if (!OBJ_HAS(o, OBJ_NPC_BEHAVIOR_SCARED) && dist <= 1.0f) {
		OBJ_SET(o, OBJ_NPC_BEHAVIOR_SCARED);
		OBJ_REM(o, OBJ_NPC_BEHAVIOR_AGGRESSIVE);

		gold_stolen = ROLL(15 * floor);
		o->gold = MIN(target->gold, gold_stolen);
		target->gold -= o->gold;
	}
	else {
		move = object_npc_update(o);

		if (OBJ_HAS(o, OBJ_NPC_IDENTIFIED_PLAYER)) {
			target = object_at(o->x + move.x, o->y + move.y);

			if (NULL != target && target->type == OBJ_DOOR
			 && OBJ_HAS(target, OBJ_DOOR_CLOSED))
				object_door_toggle(target);
		}
	}
}

void
object_goblin_killed(object_t *o)
{
	OBJ_REM(o, OBJ_IS_COLLIDABLE);
	OBJ_SET(o, OBJ_NPC_DEAD);
	o->ent = ENT_GOBLIN_DEAD;

	object_drop(o);
}
