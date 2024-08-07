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

#ifndef RLONE_OBJECT_DOOR_H
#define RLONE_OBJECT_DOOR_H

#include "object.h"

/* Door flags. */
#define OBJ_DOOR_CLOSED		(0x1)
#define OBJ_DOOR_LOCKED		(0x1 << 1)

void	object_door_init(object_t *);
void	object_door_toggle(object_t *);
void	object_door_unlock(object_t *);
void	object_door_killed(object_t *);

#endif
