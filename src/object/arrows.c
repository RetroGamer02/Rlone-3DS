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

#include "object_arrows.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "util.h"
#include "log.h"

static dialog_t	*inspect;

void
object_arrows_init(object_t *o)
{
	o->ent = ENT_ARROW;

	switch (o->type) {
	case OBJ_ARROWS:
		o->ammo = RANGE(10, 30);
		break;
	case OBJ_ARROWS_LOTS:
		o->ammo = RANGE(30, 255);
		o->type = OBJ_ARROWS;
		break;
	default:
		warn("Well this is unexpected.\n", NULL);
		break;
	}

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"A bundle of arrows.", NULL
		);
	}

	o->inspect = inspect;
}
