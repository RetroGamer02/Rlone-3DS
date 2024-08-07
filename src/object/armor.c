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
#include <string.h>
#include <math.h>

#include "renderer_entity.h"
#include "renderer.h"

#include "object_armor.h"
#include "util.h"
#include "log.h"

const tier_weight_t	armor_tier[] = {
	{ OBJ_ARMOR_LEGEND,   1 },
	{   OBJ_ARMOR_RARE,  99 },
	{ OBJ_ARMOR_COMMON, 900 }
};
const	int			armor_tier_len = 3;
#define	ARMOR_ROLL	1000

enum object
_armor_roll_type()
{
	int roll = ROLL(ARMOR_ROLL),
		next_roll;
	int	floor = object_get_floor();
	int i;

	while (floor-- > 0) {
		next_roll = ROLL(ARMOR_ROLL);
		if (next_roll > roll)
			roll = next_roll;
	}

	for (i = 0; i < armor_tier_len; i++) {
		if (roll >= (ARMOR_ROLL - armor_tier[i].weight))
			return armor_tier[i].type;
	}

	/* Return the lowest option. */
	return armor_tier[armor_tier_len - 1].type;
}

void
_armor_roll_values(object_t *o, int hp, int ac)
{
	int	hp1, hp2 = 0,
		ac1, ac2 = 0,
		hp_max, ac_max;

	hp1 = ROLL(hp);
	/* Ensure we at least get 10HP */
	while (hp2 < 10)
		hp2 = ROLL(hp);

	ac1 = ROLL(ac);
	/* Ensure 0 is never rolled for either. */
	while (ac2 < 1)
		ac2 = ROLL(ac);

	hp_max = MAX(hp1, hp2);
	if (hp_max > o->hp_max)
		o->hp_max = hp_max;

	ac_max = MAX(ac1, ac2);
	if (ac_max > o->ac_max)
		o->ac_max = ac_max;
}

#define INSPECT_INTRO	"A chestplate of "
#define INSPECT_QUALITY	" quality.\n"
#define INSPECT_HP		"\x1E "
#define INSPECT_AC		" \x10 "

#define INSPECT_COMMON	"common"
#define INSPECT_RARE	"rare"
#define INSPECT_LEGEND	"legendary"

void
_armor_build_inspect(object_t *o)
{
	const size_t	 max_len =
			strlen(INSPECT_INTRO)
		  + strlen(INSPECT_LEGEND)
		  + strlen(INSPECT_QUALITY)
		  + strlen(INSPECT_HP) + 10
		  + strlen(INSPECT_AC) + 10;
	char			*msg;
	const char		*quality;

	switch (o->type) {
	case OBJ_ARMOR_LEGEND:
		quality = INSPECT_LEGEND;
		break;
	case OBJ_ARMOR_RARE:
		quality = INSPECT_RARE;
		break;
	case OBJ_ARMOR_COMMON:
		quality = INSPECT_COMMON;
		break;
	default:
		quality = "unknown";
	}

	msg = malloc(sizeof(char) * max_len);
	snprintf(msg, max_len,
		INSPECT_INTRO "%s" INSPECT_QUALITY
		INSPECT_HP "%d" INSPECT_AC "%d",
		quality, o->hp_max, o->ac_max);
	msg[max_len - 1] = 0;

	o->inspect = rend_dialog_create(msg, NULL);
}

void
object_armor_init(object_t *o)
{
	int max_hp, max_ac;
	int rolls = 1 + (object_get_floor() / 2);

	switch (o->type) {
	case OBJ_ARMOR_LEGEND:
		o->ent = ENT_ARMOR_LEGEND;
		max_hp = 80;
		max_ac = 20;
		break;
	case OBJ_ARMOR_RARE:
		o->ent = ENT_ARMOR_RARE;
		max_hp = 40;
		max_ac = 10;
		break;
	case OBJ_ARMOR_COMMON:
		o->ent = ENT_ARMOR_COMMON;
		max_hp = 20;
		max_ac = 5;
		break;
	case OBJ_ARMOR:
	default:
		/* Generate what armor_tier the armor is. */
		o->type = _armor_roll_type();
		object_armor_init(o);
		return;
	}

	while (rolls-- > 0)
		_armor_roll_values(o, max_hp, max_ac);

	_armor_build_inspect(o);

#ifdef DEBUG
	debug("Armor generated: HP %d, AP %d.\n", o->hp_max, o->ac_max);
#endif
}

void
object_armor_free(object_t *o)
{
	/* If gold is not 0, it indicates this was picked up. */
	if (o->gold == 0) {
		free(o->inspect->message);
		rend_dialog_free(o->inspect);
	}
}
