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

#ifndef RLONE_RENDERER_ENTITY_H
#define RLONE_RENDERER_ENTITY_H

typedef struct {
	unsigned char	c;
	int				r, g, b;
} rend_entity_t;

enum rend_entity {
	ENT_HEART,
	ENT_COIN,
	ENT_PLAYER,
	ENT_DOOR_CLOSED,
	ENT_DOOR_OPENED,
	ENT_DOOR_BONKED,
	ENT_ARMOR_COMMON,
	ENT_ARMOR_RARE,
	ENT_ARMOR_LEGEND,
	ENT_SWORD_COMMON,
	ENT_SWORD_RARE,
	ENT_SWORD_LEGEND,
	ENT_BOW_COMMON,
	ENT_BOW_RARE,
	ENT_BOW_LEGEND,
	ENT_ARROW,
	ENT_NOTFOUND,
	ENT_CHEST,
	ENT_INSPECTER,
	ENT_SLASH,
	ENT_HELMET,
	ENT_SHIELD,
	ENT_CTRL_ARROWS_LEFT,
	ENT_CTRL_ARROWS_RIGHT,
	ENT_HEALTH_POTION,
	ENT_COLUMN,
	ENT_COLUMN_BROKEN,
	ENT_ZOMBIE,
	ENT_ZOMBIE_DEAD,
	ENT_STAIRS,
	ENT_STATUE,
	ENT_STATUE_DEAD,
	ENT_GOBLIN,
	ENT_GOBLIN_DEAD,
	ENT_PSV_CIRCLE,
	ENT_PSV_CROSS,
	ENT_PSV_SQUARE,
	ENT_PSV_TRIANGLE,
	ENT_PSV_START,
	ENT_PSV_SELECT,
	ENT_PSV_LTRIGGER,
	ENT_PSV_RTRIGGER
};

const rend_entity_t	*rend_entity_get(enum rend_entity);
const rend_entity_t	*rend_entity_for(char);

#endif
