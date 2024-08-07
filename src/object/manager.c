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

#include "object_manager.h"

void
object_manager_init(object_t *m)
{
	m->rend_flags = OBJ_REND_DISABLED;
	m->flags = OBJ_MANAGER_AUTO_GC;
}

void
object_manager_update(object_t *m)
{
	if (m->flags == OBJ_MANAGER_AUTO_GC)
		object_gc();
}
