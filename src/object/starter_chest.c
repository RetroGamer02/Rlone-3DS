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

#include "object_starter_chest.h"
#include "object_chest.h"
#include "util.h"

static dialog_t	*inspect;

#define ITEMS_LEN	5
struct loadout {
	int			weight;
	enum object	items[ITEMS_LEN];
};

#define LOADOUTS_LEN	5
const struct loadout loadouts[LOADOUTS_LEN] = {
	/* All legendary. */
	{      1, { OBJ_HEALTH_POTION, OBJ_SWORD_LEGEND, OBJ_ARMOR_LEGEND, OBJ_BOW_LEGEND, OBJ_ARROWS_LOTS } },
	/* Only armor. Good luck. */
	{  44444, { OBJ_ARMOR_RARE } },
	/* Both weapons. */
	{  55555, { OBJ_HEALTH_POTION, OBJ_SWORD, OBJ_ARMOR, OBJ_BOW, OBJ_ARROWS } },
	/* Arrow loadout. */
	{ 300000, { OBJ_HEALTH_POTION, OBJ_ARMOR, OBJ_BOW, OBJ_ARROWS } },
	/* Sword loadout. */
	{ 600000, { OBJ_HEALTH_POTION, OBJ_SWORD, OBJ_ARMOR} }
};
#define LOADOUT_ROLL	1000000

#ifndef DEBUG
static const struct loadout *
_starter_loadout_roll()
{
	int roll = ROLL(LOADOUT_ROLL);
	int i;

	for (i = 0; i < LOADOUTS_LEN; i++) {
		if (roll >= (LOADOUT_ROLL - loadouts[i].weight))
			return loadouts + i;
	}

	/* Return the default loadout. */
	return loadouts + LOADOUTS_LEN - 1;
}
#endif

void
object_starter_chest_init(object_t *o)
{
	const struct loadout	*chosen;
	int						 i;

	object_chest_init(o);

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A chest full of starting gear.", NULL
		);
	}

	o->inspect = inspect;

	/* TODO: Generate the actual starting gear. */
#if DEBUG
	chosen = loadouts;
#else
	chosen = _starter_loadout_roll();
#endif

	for (i = 0; i < ITEMS_LEN; i++) {
		if (chosen->items[i] == 0)
			break;

		o->drops[i] = chosen->items[i];
		o->drops_len++;
	}
}
