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

#include "object_projectile.h"
#include "object_projectile_arrow.h"

object_t *
object_projectile_shoot(enum object type,
						int x, int y,
						int mx, int my,
						int dl, int dh,
						int al, int ah)
{
	object_t *o;

	if (object_collision_at(x, y))
		return NULL;

	o = object_create(type, x, y);

	if (NULL != o) {
		/* Re-gather the player object. */
		switch (mx) {
		case -1:
			OBJ_SET(o, OBJ_PROJECTILE_LEFT);
			break;
		case 1:
			OBJ_SET(o, OBJ_PROJECTILE_RIGHT);
			break;
		default:
			break;
		}

		switch (my) {
		case -1:
			OBJ_SET(o, OBJ_PROJECTILE_UP);
			break;
		case 1:
			OBJ_SET(o, OBJ_PROJECTILE_DOWN);
			break;
		default:
			break;
		}

		/* Set the slashes damage and armor values. */
		o->hp = dl;
		o->ac = al;
		o->hp_max = dh;
		o->ac_max = ah;

		/* Set the mx and my, in case it's a PATHED projectile. */
		o->dmg = mx;
		o->dmg_max = my;
	}

	return o;
}
