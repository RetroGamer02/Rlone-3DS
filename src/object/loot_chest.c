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

#include "object_loot_chest.h"
#include "object_chest.h"
#include "util.h"

static dialog_t	*inspect;

#define ITEMS_LEN	5
struct loadout {
	int			weight;
	enum object	items[ITEMS_LEN];
};

#define LOADOUTS_LEN	7
const struct loadout loot_chest_table[LOADOUTS_LEN] = {
	/* All legendary. */
	{      1, { OBJ_HEALTH_POTION, OBJ_SWORD_LEGEND, OBJ_ARMOR_LEGEND, OBJ_BOW_LEGEND, OBJ_ARROWS_LOTS } },
	/* Only armor. Good luck. */
	{  44444, { OBJ_ARMOR_RARE } },
	/* Both weapons. */
	{  55555, { OBJ_HEALTH_POTION, OBJ_SWORD, OBJ_ARMOR, OBJ_BOW, OBJ_ARROWS } },
	/* Arrow loadout. */
	{ 50000, { OBJ_HEALTH_POTION, OBJ_ARMOR, OBJ_BOW, OBJ_ARROWS } },
	/* Sword loadout. */
	{ 100000, { OBJ_HEALTH_POTION, OBJ_SWORD, OBJ_ARMOR} },
	/* Supplies. */
	{ 350000, { OBJ_ARROWS, OBJ_HEALTH_POTION, OBJ_ARROWS } },
	/* Only gold. */
	{ 400000, { OBJ_GOLD, OBJ_GOLD, OBJ_GOLD } },
};
#define LOADOUT_ROLL	1000000

static const struct loadout *
_starter_loadout_roll()
{
	int roll = ROLL(LOADOUT_ROLL);
	int i;

	for (i = 0; i < LOADOUTS_LEN; i++) {
		if (roll >= (LOADOUT_ROLL - loot_chest_table[i].weight))
			return loot_chest_table + i;
	}

	/* Return the default loadout. */
	return loot_chest_table + LOADOUTS_LEN - 1;
}

void
object_loot_chest_init(object_t *o)
{
	const struct loadout	*chosen;
	int						 i;

	object_chest_init(o);

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A chest with loot.\nA loot chest, if you will.", NULL
		);
	}

	o->inspect = inspect;

	chosen = _starter_loadout_roll();

	for (i = 0; i < ITEMS_LEN; i++) {
		if (chosen->items[i] == 0)
			break;

		o->drops[i] = chosen->items[i];
		o->drops_len++;
	}
}
