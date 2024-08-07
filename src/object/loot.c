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

#include "object_loot.h"

#include "util.h"
#include "log.h"

#define LOOT_TABLE_LEN	9
#define LOOT_TABLE_ROLL	200
const tier_weight_t	loot_table[LOOT_TABLE_LEN] = {
	{ OBJ_LOOT_CHEST,    40 },
	{ OBJ_SWORD,         10 },
	{ OBJ_BOW,           10 },
	{ OBJ_ARMOR,         10 },
	{ OBJ_ARROWS_LOTS,    5 },
	{ OBJ_ARROWS,        20 },
	{ OBJ_HEALTH_POTION, 10 },
	{ OBJ_GOLD,          94 },
	{ OBJ_CHEST,          1 },
};

void
object_loot_init(object_t *o)
{
	enum object		loot = 0;
	int				i, roll;

	roll = ROLL(LOOT_TABLE_ROLL);
	for (i = 0; i < LOOT_TABLE_LEN; i++) {
		roll -= loot_table[i].weight;
		if (roll < 0) {
			loot = loot_table[i].type;
			break;
		}
	}

	if (loot == 0)
		loot = loot_table[0].type;

#ifdef DEBUG
	debug("Generating loot of type %d at (%d,%d).\n", loot, o->x, o->y);
#endif

	o->enabled = 0;
	object_create(loot, o->x, o->y);
}
