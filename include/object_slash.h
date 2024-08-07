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

#ifndef RLONE_OBJECT_SLASH_H
#define RLONE_OBJECT_SLASH_H

#include "object.h"

#define	OBJ_SLASH_HAS_STRUCK	((uint64_t)0x1 << 32)

void	object_slash_init(object_t *);
void	object_slash_update(object_t *);
void	object_slash_create(int, int, int, int, int, int);

#endif
