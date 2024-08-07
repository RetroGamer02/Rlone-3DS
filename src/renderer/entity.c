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

#include <stddef.h>

#include "renderer_entity.h"

/* TODO: If this operation ever gets unwieldy, pre-sort the array and use a
   binary search. */

#define rend_entities_len	(sizeof(rend_entities) / sizeof(rend_entities[0]))
const rend_entity_t	rend_entities[] = {
	[ENT_HEART]				= {  30, 255,   0,   0 },
	[ENT_COIN]				= {  29, 255, 215,   0 },
	[ENT_PLAYER]			= {  28, 212, 128, 255 },
	[ENT_DOOR_CLOSED]		= {  27, 128,  64,  64 },
	[ENT_DOOR_OPENED]		= {  26, 128,  64,  64 },
	[ENT_DOOR_BONKED]		= {  18, 128,  64,  64 },
	[ENT_ARMOR_COMMON]		= {  16, 132, 140, 150 },
	[ENT_ARMOR_RARE]		= {  16, 174, 187, 219 },
	[ENT_ARMOR_LEGEND]		= {  16, 255, 225, 205 },
	[ENT_SWORD_COMMON]		= {  24, 132, 140, 150 },
	[ENT_SWORD_RARE]		= {  24, 174, 187, 219 },
	[ENT_SWORD_LEGEND]		= {  24, 255, 225, 205 },
	[ENT_BOW_COMMON]		= {  12, 128,  64,  64 },
	[ENT_BOW_RARE]			= {  12, 174, 187, 219 },
	[ENT_BOW_LEGEND]		= {  12, 255, 225, 205 },
	[ENT_ARROW]				= {  23, 132, 140, 150 },
	[ENT_NOTFOUND]			= {  22, 255,   0,   0 },
	[ENT_CHEST]				= {  21, 132, 140, 150 },
	[ENT_INSPECTER]			= {  20, 255, 255, 255 },
	[ENT_SLASH]				= {  19, 255, 215, 165 },
	[ENT_HELMET]			= {  25, 132, 140, 150 },
	[ENT_SHIELD]			= {  17, 132, 140, 150 },
	[ENT_CTRL_ARROWS_LEFT]	= {   1, 255, 255, 255 },
	[ENT_CTRL_ARROWS_RIGHT]	= {   2, 255, 255, 255 },
	[ENT_HEALTH_POTION]		= {  15, 255,  64,  64 },
	[ENT_COLUMN]			= {  14, 255, 255, 255 },
	[ENT_COLUMN_BROKEN]		= {  13, 225, 225, 225 },
	[ENT_ZOMBIE]			= {  11, 192, 192, 128 },
	[ENT_ZOMBIE_DEAD]		= {  10,  96,  96,  64 },
	[ENT_STAIRS]			= {   9, 255, 255, 255 },
	[ENT_STATUE]			= {   8, 235, 235, 235 },
	[ENT_STATUE_DEAD]		= {   7, 205, 205, 205 },
	[ENT_GOBLIN]			= {   6, 192, 192,  32 },
	[ENT_GOBLIN_DEAD]		= {   5,  96,  96,  16 },
	[ENT_PSV_CIRCLE]		= { 253, 255, 102, 102 },
	[ENT_PSV_CROSS]			= { 254, 124, 178, 232 },
	[ENT_PSV_SQUARE]		= { 252, 255, 105, 248 },
	[ENT_PSV_TRIANGLE]		= { 251,  64, 226, 160 },
	[ENT_PSV_START]			= { 250, 255, 255, 255 },
	[ENT_PSV_SELECT]		= { 249, 255, 255, 255 },
	[ENT_PSV_LTRIGGER]		= { 248, 255, 255, 255 },
	[ENT_PSV_RTRIGGER]		= { 247, 255, 255, 255 },
};

const rend_entity_t *
rend_entity_get(enum rend_entity e)
{
	return rend_entities + e;
}

const rend_entity_t *
rend_entity_for(char chr)
{
	size_t i;

	for (i = 0; i < rend_entities_len; i++) {
		if (rend_entities[i].c == chr)
			return rend_entities + i;
	}

	return NULL;
}
