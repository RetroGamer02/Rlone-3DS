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

#ifndef RLONE_OBJECT_PLAYER_H
#define RLONE_OBJECT_PLAYER_H

#include "object.h"

#define OBJ_PLAYER_ACTION_INSPECT		((uint64_t)0x1 << 3)
#define OBJ_PLAYER_ACTION_INTERACT		((uint64_t)0x1 << 4)
#define OBJ_PLAYER_ACTION_INTERACTED	((uint64_t)0x1 << 5)
#define OBJ_PLAYER_ACTION_SLASH			((uint64_t)0x1 << 6)
#define OBJ_PLAYER_ACTION_SLASHED		((uint64_t)0x1 << 7)
#define OBJ_PLAYER_ACTION_SHOOT			((uint64_t)0x1 << 8)
#define OBJ_PLAYER_ACTION_SHOOTED		((uint64_t)0x1 << 9)
#define OBJ_PLAYER_SWORD				((uint64_t)0x1 << 32)
#define OBJ_PLAYER_BOW					((uint64_t)0x1 << 33)
#define OBJ_PLAYER_NEXT_FLOOR			((uint64_t)0x1 << 34)
#define OBJ_PLAYER_DEAD					((uint64_t)0x1 << 40)

void	object_player_init(object_t *);
void	object_player_update(object_t *);
void	object_player_killed(object_t *);
int		object_player_get_armor_entity();
int		object_player_get_sword_entity();
int		object_player_get_bow_entity();
int		object_player_get_health_potions();

#endif
