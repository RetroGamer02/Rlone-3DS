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

#ifndef RLONE_MAP_H
#define RLONE_MAP_H

#include "map_type.h"
#include "map_gen.h"

#include "object.h"

#ifdef DEBUG
#include "map_debug.h"
#endif
#include "map_default.h"

void	map_init_rand();
char	map_get_at(map_t *, int, int);
char	map_set_at(map_t *, int, int, char);
size_t	map_new_object(map_t *, enum object, int, int);
void	map_free(map_t *);

#endif
