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

#include "object_chest.h"

#include "renderer_dialog.h"
#include "log.h"

static dialog_t	*inspect;

void
object_chest_init(object_t *o)
{
	o->ent = ENT_CHEST;
	o->flags = OBJ_IS_COLLIDABLE;

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A suspiciously lightweight chest.", NULL
		);
	}

	o->inspect = inspect;

	o->hp = o->hp_max = 75;
	o->ac = o->ac_max = 15;
}

void
object_chest_open(object_t *o)
{
	o->enabled = 0;
	object_drop(o);

#ifdef DEBUG
	debug("Chest at (%d,%d) opened.\n", o->x, o->y);
#endif
}

void
object_chest_killed(object_t *o)
{
	object_chest_open(o);
}
