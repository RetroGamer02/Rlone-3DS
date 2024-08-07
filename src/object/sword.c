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

#include "renderer_entity.h"
#include "renderer.h"

#include "object_sword.h"
#include "util.h"
#include "log.h"

const tier_weight_t	sword_tier[] = {
	{ OBJ_SWORD_LEGEND,   1 },
	{   OBJ_SWORD_RARE,  99 },
	{ OBJ_SWORD_COMMON, 900 }
};
const	int			sword_tier_len = 3;
#define	SWORD_ROLL	1000

enum object
_sword_roll_type()
{
	int roll = ROLL(SWORD_ROLL),
		next_roll;
	int	floor = object_get_floor();
	int i;

	while (floor-- > 0) {
		next_roll = ROLL(SWORD_ROLL);
		if (next_roll > roll)
			roll = next_roll;
	}

	for (i = 0; i < sword_tier_len; i++) {
		if (roll >= (SWORD_ROLL - sword_tier[i].weight))
			return sword_tier[i].type;
	}

	/* Return the lowest option. */
	return sword_tier[sword_tier_len - 1].type;
}

void
_sword_roll_values(object_t *o, int dmg, int pen)
{
	int	dmg1, dmg2, pen1, pen2;
	int hp, hp_max, ac, ac_max;

	dmg1 = dmg2 = ROLL(dmg);
	while (dmg2 == dmg1)
		dmg2 = ROLL(dmg);

	pen1 = pen2 = ROLL(pen);
	while (pen2 == pen1)
		pen2 = ROLL(pen);

	hp     = MIN(dmg1, dmg2);
	if (hp > o->hp)
		o->hp = hp;

	hp_max = MAX(dmg1, dmg2);
	if (hp_max > o->hp_max)
		o->hp_max = hp_max;

	ac     = MIN(pen1, pen2);
	if (ac > o->ac)
		o->ac = ac;

	ac_max = MAX(pen1, pen2);
	if (ac_max > o->ac_max)
		o->ac_max = ac_max;

#ifdef DEBUG
	debug("Sword generated: DMG %d-%d, AP %d-%d.\n", o->hp, o->hp_max, o->ac, o->ac_max);
#endif
}

#define INSPECT_INTRO	"A sword of "
#define INSPECT_QUALITY	" quality.\n"
#define INSPECT_DMG		"\x18 "
#define INSPECT_PEN		" \x19 "

#define INSPECT_COMMON	"common"
#define INSPECT_RARE	"rare"
#define INSPECT_LEGEND	"legendary"

void
_sword_build_inspect(object_t *o)
{
	const size_t	 max_len =
			strlen(INSPECT_INTRO)
		  + strlen(INSPECT_LEGEND)
		  + strlen(INSPECT_QUALITY)
		  + strlen(INSPECT_DMG) + 24
		  + strlen(INSPECT_PEN) + 24;
	char			*msg;
	const char		*quality;

	switch (o->type) {
	case OBJ_SWORD_LEGEND:
		quality = INSPECT_LEGEND;
		break;
	case OBJ_SWORD_RARE:
		quality = INSPECT_RARE;
		break;
	case OBJ_SWORD_COMMON:
		quality = INSPECT_COMMON;
		break;
	default:
		quality = "unknown";
	}

	msg = malloc(sizeof(char) * max_len);
	snprintf(msg, max_len,
		INSPECT_INTRO "%s" INSPECT_QUALITY
		INSPECT_DMG "%d-%d" INSPECT_PEN "%d-%d",
		quality, o->hp, o->hp_max, o->ac, o->ac_max);
	msg[max_len - 1] = 0;

	o->inspect = rend_dialog_create(msg, NULL);
}

void
object_sword_init(object_t *o)
{
	int max_dmg, max_ap;
	int rolls = 1 + (object_get_floor() / 2);

	switch (o->type) {
	case OBJ_SWORD_LEGEND:
		o->ent = ENT_SWORD_LEGEND;
		max_dmg = 80;
		max_ap = 40;
		break;
	case OBJ_SWORD_RARE:
		o->ent = ENT_SWORD_RARE;
		max_dmg = 40;
		max_ap = 20;
		break;
	case OBJ_SWORD_COMMON:
		o->ent = ENT_SWORD_COMMON;
		max_dmg = 20;
		max_ap = 10;
		break;
	case OBJ_SWORD:
	default:
		/* Generate what sword_tier the sword is. */
		o->type = _sword_roll_type();
		object_sword_init(o);
		return;
	}

	while (rolls-- > 0)
		_sword_roll_values(o, max_dmg, max_ap);

	_sword_build_inspect(o);
}

void
object_sword_free(object_t *o)
{
	/* If gold is not 0, it indicates this was picked up. */
	if (o->gold == 0) {
		free(o->inspect->message);
		rend_dialog_free(o->inspect);
	}
}
