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

#include "object_column.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "util.h"

static dialog_t	*inspect,
				*inspect_bonked;

int
_column_blocks_door(object_t *o)
{
	if (object_at(o->x - 1, o->y))
		return 1;
	else if (object_at(o->x + 1, o->y))
		return 1;
	else if (object_at(o->x, o->y - 1))
		return 1;
	else if (object_at(o->x, o->y + 1))
		return 1;

	return 0;
}

void
object_column_init(object_t *o)
{
	if (_column_blocks_door(o)) {
		/* Don't generate this column. */
		o->enabled = 0;
		return;
	}

	o->ent = (ROLL(10) < 6) ? ENT_COLUMN : ENT_COLUMN_BROKEN;
	o->flags = OBJ_IS_COLLIDABLE;
	
	if (o->ent == ENT_COLUMN_BROKEN)
		OBJ_SET(o, OBJ_DESTROYED);

	o->hp = o->hp_max = 100;
	o->ac = o->ac_max =  20;

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A proudly standing marble column.", NULL
		);
	}

	if (NULL == inspect_bonked) {
		inspect_bonked = rend_dialog_create(
			"A once great column, now\nbut only rubble.", NULL
		);
	}

	o->inspect = o->ent == ENT_COLUMN ? inspect : inspect_bonked;
}

void
object_column_killed(object_t *o)
{
	OBJ_SET(o, OBJ_DESTROYED);
	o->ent = ENT_COLUMN_BROKEN;
	o->inspect = inspect_bonked;
}
