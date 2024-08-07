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

#ifndef RLONE_OBJECT_PROJECTILE_ARROW_H
#define RLONE_OBJECT_PROJECTILE_ARROW_H

#include "object.h"

#define OBJ_PROJECTILE_UP		((uint64_t)0x1 << 32)
#define OBJ_PROJECTILE_DOWN 	((uint64_t)0x1 << 33)
#define OBJ_PROJECTILE_LEFT 	((uint64_t)0x1 << 34)
#define OBJ_PROJECTILE_RIGHT	((uint64_t)0x1 << 35)

#define OBJ_PROJECTILE_PATHED	((uint64_t)0x1 << 36)

void	object_projectile_arrow_init(object_t *);
void	object_projectile_arrow_update(object_t *);

#endif
