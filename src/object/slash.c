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

#include "object_slash.h"

#include "renderer_entity.h"
#include "util.h"

void
object_slash_init(object_t *o)
{
	o->ent = ENT_SLASH;

	/* Ensure we attack immediately. */
	o->us_a = 0.1f;
}

void
object_slash_update(object_t *o)
{
	object_t	*t;

	if (OBJ_HAS(o, OBJ_SLASH_HAS_STRUCK))
		o->enabled = 0;
	else {
		/* Assume whatever created us has set the appropriate values. */
		t = object_at(o->x, o->y);

		if (NULL != t && OBJ_HAS(t, OBJ_IS_COLLIDABLE)) {
			object_hit(
				t,
				RANGE(o->hp, o->hp_max),
				RANGE(o->ac, o->ac_max)
			);
		}

		OBJ_SET(o, OBJ_SLASH_HAS_STRUCK);
	}
}

void
object_slash_create(int x, int y, int dl, int dh, int al, int ah)
{
	object_t *o = object_create(OBJ_SLASH, x, y);

	if (NULL != o) {
		/* Set the slashes damage and armor values. */
		o->hp = dl;
		o->ac = al;
		o->hp_max = dh;
		o->ac_max = ah;
	}
}
