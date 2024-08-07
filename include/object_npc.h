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

#ifndef RLONE_OBJECT_NPC_H
#define RLONE_OBJECT_NPC_H

#include "object.h"

#define OBJECT

/* The flag indicating whether the NPC should wander, or
   activate it's response to the player. */
#define OBJ_NPC_IDENTIFIED_PLAYER	((uint64_t)0x1 << 32)

/* This flag indicates if the NPC will wander by default, or
   remain stationary. */
#define OBJ_NPC_WANDERS				((uint64_t)0x1 << 33)

/* The behavior for if the player has been found. */
#define	OBJ_NPC_BEHAVIOR_PASSIVE	((uint64_t)0x1 << 34)
#define	OBJ_NPC_BEHAVIOR_AGGRESSIVE	((uint64_t)0x1 << 35)
#define	OBJ_NPC_BEHAVIOR_SCARED		((uint64_t)0x1 << 36)

/* Current pathfinding flags. */
#define OBJ_NPC_STUCK				((uint64_t)0x1 << 37)

/* Keep the "correct_facing" algorithm from changing if we
   have yet to reach the door. Overriden by STUCK. */
#define OBJ_NPC_HAS_DOOR_X			((uint64_t)0x1 << 38)
#define OBJ_NPC_HAS_DOOR_Y			((uint64_t)0x1 << 39)

/* Is the NPC dead? */
#define OBJ_NPC_DEAD				((uint64_t)0x1 << 40)

void		object_npc_init(object_t *);
SDL_Point	object_npc_update(object_t *);
SDL_Point	object_npc_pathfind_to(object_t *, object_t *);
SDL_Point	object_npc_pathfind_to_point(object_t *, int, int);
SDL_Point	object_npc_wander(object_t *);
SDL_Point	object_npc_raycast_to(object_t *, int, int);
float		object_npc_distance(object_t *, object_t *);

#endif
