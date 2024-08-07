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

#include "object_door.h"

#include "renderer_dialog.h"
#include "renderer_entity.h"
#include "renderer.h"
#include "util.h"
#include "log.h"

static dialog_t	*inspect_open;
static dialog_t	*inspect_closed;
static dialog_t	*inspect_bonked;

void
object_door_init(object_t *o)
{
	o->ent		= ENT_DOOR_CLOSED;
	o->flags	= OBJ_DOOR_CLOSED;

	if (NULL == inspect_closed) {
		inspect_closed = rend_dialog_create(
			"A closed, wooden door.", NULL
		);
	}

	if (NULL == inspect_open) {
		inspect_open = rend_dialog_create(
			"An open, wooden door.", NULL
		);
	}

	if (NULL == inspect_bonked) {
		inspect_bonked = rend_dialog_create(
			"The gruesome remains of a poor,\nbroken door.", NULL
		);
	}

	o->hp = o->hp_max = 30;
	o->ac = o->ac_max = 10;

	o->inspect = inspect_closed;
}

void
object_door_toggle(object_t *o)
{
	if (OBJ_HAS(o, OBJ_DESTROYED))
		return;

	o->flags ^= OBJ_DOOR_CLOSED;
	if (OBJ_HAS(o, OBJ_DOOR_CLOSED)) {
		o->ent = ENT_DOOR_CLOSED;
		o->inspect = inspect_closed;
	}
	else {
		o->ent = ENT_DOOR_OPENED;
		o->inspect = inspect_open;
	}
}

void
object_door_unlock(object_t *o)
{
	if (OBJ_HAS(o, OBJ_DESTROYED))
		return;

	OBJ_REM(o, OBJ_DOOR_LOCKED);
}

void
object_door_killed(object_t *o)
{
	OBJ_SET(o, OBJ_DESTROYED);
	OBJ_REM(o, OBJ_DOOR_CLOSED);
	o->ent = ENT_DOOR_BONKED;
	o->inspect = inspect_bonked;
}
