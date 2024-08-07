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

#include "object_statue.h"
#include "object_npc.h"

#include "object_projectile.h"

#include "renderer_dialog.h"
#include "renderer_entity.h"
#include "util.h"

static dialog_t	*inspect;
static dialog_t	*inspect_dead;

void
object_statue_init(object_t *o)
{
	int	floor = object_get_floor();

	object_npc_init(o);

	o->ent = ENT_STATUE;

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A suspicious statue with a strung bow.", NULL
		);
	}
	else if (NULL == inspect_dead) {
		inspect_dead = rend_dialog_create(
			"A collapsed statue of an archer.", NULL
		);
	}

	OBJ_SET(o, OBJ_NPC_BEHAVIOR_AGGRESSIVE);
	OBJ_SET(o, OBJ_NPC_IDENTIFIED_PLAYER);

	/* Roll our statue! */
	o->hp = o->hp_max = RANGE(5 * floor, 20 * floor);
	o->ac = 0;
	o->ac_max = RANGE(7 * floor, 14 * floor);

	o->dmg = o->ap = 0;
	o->dmg_max = RANGE(2 * floor, 5 * floor);
	o->ap_max = RANGE(4 * floor, 9 * floor);

	o->inspect = inspect;

	if (ROLL(100) < 90) {
		o->drops[0] = OBJ_ARROWS;
		o->drops_len++;
	}

	if (ROLL(100) < 10) {
		o->drops[o->drops_len] = OBJ_BOW;
		o->drops_len++;
	}
}

void
object_statue_update(object_t *o)
{
	SDL_Point	 raypos,
				 move,
				 to;
	object_t	*player;

	if (OBJ_HAS(o, OBJ_NPC_DEAD))
		return;

	if (OBJ_HAS(o, OBJ_NPC_IDENTIFIED_PLAYER)) {
		player = object_first(OBJ_PLAYER);
		raypos = object_npc_raycast_to(o, player->x, player->y);

		if (abs(player->x - o->x) > 1 || abs(player->y - o->y) > 1) {
			if (abs(player->x - o->x) > abs(player->y - o->y))
				to = (SDL_Point) { player->x + RANGE(-1, 3), player->y };
			else if (abs(player->x - o->x) < abs(player->y - o->y))
				to = (SDL_Point) { player->x, player->y + RANGE(-1, 3) };
			else
				to = (SDL_Point) { player->x, player->y };
		}
		else
			to = (SDL_Point) { player->x, player->y };

		if (raypos.y == player->y && raypos.x == player->x) {
			move = (SDL_Point){
				MIN(1, MAX(-1, to.x - o->x)),
				MIN(1, MAX(-1, to.y - o->y))
			};

			raypos.x = move.x + o->x;
			raypos.y = move.y + o->y;

			object_projectile_shoot(
				OBJ_PROJECTILE_ARROW_PATHED,
				raypos.x, raypos.y,
				to.x, to.y,
				o->dmg, o->dmg_max,
				o->ap, o->ap_max
			);
		}
	}
}

void
object_statue_killed(object_t *o)
{
	OBJ_REM(o, OBJ_IS_COLLIDABLE);
	OBJ_SET(o, OBJ_NPC_DEAD);
	o->ent = ENT_STATUE_DEAD;
	o->inspect = inspect_dead;

	object_drop(o);
}
