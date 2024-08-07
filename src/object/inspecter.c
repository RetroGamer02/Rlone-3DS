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

#include "object_inspecter.h"

#include "renderer_dialog.h"
#include "renderer_entity.h"
#include "renderer.h"
#include "input.h"

#define OBJ_INSPECTER_MAX_RANGE	7

static dialog_t	*oor_inspect;

void
object_inspecter_init(object_t *o)
{
	if (NULL == oor_inspect) {
		oor_inspect = rend_dialog_create(
			"The object is out of view.", NULL
		);
	}

	o->ent = ENT_INSPECTER;
}

void
object_inspecter_update(object_t *o)
{
	object_t	*player, *target;
	int			 dist;

	if (input_is_down(IN_MOVE_UP))
		o->y--;
	if (input_is_down(IN_MOVE_DOWN))
		o->y++;
	if (input_is_down(IN_MOVE_LEFT))
		o->x--;
	if (input_is_down(IN_MOVE_RIGHT))
		o->x++;

	if (o->gold == 1 && (
		input_clicked(IN_CONTINUE) || input_clicked(IN_INSPECT))) {
		player = object_first(OBJ_PLAYER);
		target = object_at(o->x, o->y);
		if (NULL == target)
			return;

		dist = object_distance(player, target);

		if (dist < OBJ_INSPECTER_MAX_RANGE) {
			if (target->inspect)
				rend_set_dialog(target->inspect);
		}
		else
			rend_set_dialog(oor_inspect);
	}

#ifdef VITABUILD
	if (input_clicked(IN_CANCEL))
#else
	if (input_clicked(IN_PAUSE))
#endif
		o->enabled = 0;

	o->gold = 1;
}
