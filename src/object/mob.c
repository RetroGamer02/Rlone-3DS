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

#include "object_mob.h"

#include "renderer_entity.h"
#include "util.h"
#include "log.h"

#define MOB_TABLE_LEN	3
#define MOB_TABLE_ROLL	150
const tier_weight_t	mob_table[MOB_TABLE_LEN] = {
	{ OBJ_NPC_ZOMBIE, 100 },
	{ OBJ_NPC_STATUE,  40 },
	{ OBJ_NPC_GOBLIN,  10 },
};

void
object_mob_init(object_t *o)
{
	enum object		mob = 0;
	int				i, roll;

	roll = ROLL(MOB_TABLE_ROLL);
	for (i = 0; i < MOB_TABLE_LEN; i++) {
		roll -= mob_table[i].weight;
		if (roll < 0) {
			mob = mob_table[i].type;
			break;
		}
	}

	if (mob == 0)
		mob = mob_table[0].type;

#ifdef DEBUG
	debug("Generating mob of type %d at (%d,%d).\n", mob, o->x, o->y);
#endif

	o->enabled = 0;
	object_create(mob, o->x, o->y);
}
