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

#include <stdlib.h>
#include <string.h>

#include "renderer.h"
#include "object.h"
#include "input.h"
#include "util.h"
#include "map.h"
#include "log.h"

/* The actual game objects. */
#include "object_player.h"
#include "object_inspecter.h"
#include "object_manager.h"
#include "object_door.h"
#include "object_chest.h"
#include "object_starter_chest.h"
#include "object_loot_chest.h"
#include "object_armor.h"
#include "object_sword.h"
#include "object_bow.h"
#include "object_slash.h"
#include "object_health_potion.h"
#include "object_column.h"
#include "object_ticker.h"
#include "object_arrows.h"
#include "object_projectile_arrow.h"
#include "object_zombie.h"
#include "object_statue.h"
#include "object_goblin.h"
#include "object_loot.h"
#include "object_gold.h"
#include "object_mob.h"
#include "object_stairs.h"
#include "object_npc.h"

const float	update_speeds[] = {
	[OBJ_PLAYER]			= 0.15f,
	[OBJ_MANAGER]			= 4.0f,
	[OBJ_INSPECTER]			= 0.1f,
	[OBJ_SLASH]				= 0.1f,
	[OBJ_TICKER]			= 6.0f,
	[OBJ_PROJECTILE_ARROW]	= 0.05f,
	[OBJ_NPC_ZOMBIE]		= 0.5f,
	[OBJ_NPC_STATUE]		= 1.0f,
	[OBJ_NPC_GOBLIN]		= 0.35f,
#ifdef DEBUG
	[OBJ_NPC_DEBUG]			= 0.5f,
#endif
};

static map_t	*map;
static size_t	 o_iter;

void
object_setmap(void *m)
{
	map = (map_t *)m;
}

#define _OBJ_US_A_OFF(t) ((rand() / (float)RAND_MAX) * update_speeds[t])

void
object_init(object_t *o)
{
	/* By default, objects should be enabled. */
	o->enabled = 1;

	/* If the object is expected to be updated,
	   give it an offset. */
	switch (o->type) {
	case OBJ_NPC_ZOMBIE:
	case OBJ_NPC_STATUE:
	case OBJ_NPC_GOBLIN:
		o->us_a = _OBJ_US_A_OFF(o->type);
		break;
	default:
		o->us_a = 0;
	}

	/* Delegate to the correct init function. */
	switch (o->type) {
	case OBJ_PLAYER:
		object_player_init(o);
		break;
	case OBJ_STAIRS:
		object_stairs_init(o);
		break;
	case OBJ_INSPECTER:
		object_inspecter_init(o);
		break;
	case OBJ_DOOR:
		object_door_init(o);
		break;
	case OBJ_CHEST:
		object_chest_init(o);
		break;
	case OBJ_STARTER_CHEST:
		object_starter_chest_init(o);
		break;
	case OBJ_LOOT_CHEST:
		object_loot_chest_init(o);
		break;
	case OBJ_COLUMN:
		object_column_init(o);
		break;
	case OBJ_LOOT:
		object_loot_init(o);
		break;
	case OBJ_MOB:
		object_mob_init(o);
		break;
	case OBJ_GOLD:
		object_gold_init(o);
		break;
	case OBJ_ARMOR_LEGEND:
	case OBJ_ARMOR_COMMON:
	case OBJ_ARMOR_RARE:
	case OBJ_ARMOR:
		object_armor_init(o);
		break;
	case OBJ_SWORD_LEGEND:
	case OBJ_SWORD_COMMON:
	case OBJ_SWORD_RARE:
	case OBJ_SWORD:
		object_sword_init(o);
		break;
	case OBJ_BOW_LEGEND:
	case OBJ_BOW_COMMON:
	case OBJ_BOW_RARE:
	case OBJ_BOW:
		object_bow_init(o);
		break;
	case OBJ_MANAGER:
		object_manager_init(o);
		break;
	case OBJ_SLASH:
		object_slash_init(o);
		break;
	case OBJ_HEALTH_POTION:
		object_health_potion_init(o);
		break;
	case OBJ_TICKER:
		object_ticker_init(o);
		break;
	case OBJ_ARROWS:
	case OBJ_ARROWS_LOTS:
		object_arrows_init(o);
		break;
	case OBJ_PROJECTILE_ARROW:
	case OBJ_PROJECTILE_ARROW_PATHED:
		object_projectile_arrow_init(o);
		break;
	case OBJ_NPC_ZOMBIE:
		object_zombie_init(o);
		break;
	case OBJ_NPC_STATUE:
		object_statue_init(o);
		break;
	case OBJ_NPC_GOBLIN:
		object_goblin_init(o);
		break;
#ifdef DEBUG
	case OBJ_NPC_DEBUG:
		object_npc_init(o);
		break;
#endif
	}
}

void
object_update(object_t *o, float dt)
{
	if (!o->enabled)
		return;

	if ((o->type == OBJ_PLAYER || o->type == OBJ_INSPECTER)
	 && input_has_change()) {
		/* Ensure the moment an update is available, the player will react. */
		o->us_a = update_speeds[o->type];
	}

	o->us_a += dt;

	/* Only update if we've waited long enough. */
	if (o->us_a < update_speeds[o->type])
		return;

	o->us_a -= update_speeds[o->type];

	/* Delegate to the correct update function. */
	switch (o->type) {
	case OBJ_PLAYER:
		object_player_update(o);
		break;
	case OBJ_INSPECTER:
		object_inspecter_update(o);
		break;
	case OBJ_MANAGER:
		object_manager_update(o);
		break;
	case OBJ_SLASH:
		object_slash_update(o);
		break;
	case OBJ_TICKER:
		object_ticker_update(o);
		break;
	case OBJ_PROJECTILE_ARROW:
		object_projectile_arrow_update(o);
		break;
	case OBJ_NPC_ZOMBIE:
		object_zombie_update(o);
		break;
	case OBJ_NPC_STATUE:
		object_statue_update(o);
		break;
	case OBJ_NPC_GOBLIN:
		object_goblin_update(o);
		break;
#ifdef DEBUG
	case OBJ_NPC_DEBUG:
		object_npc_update(o);
		break;
#endif
	default:
		break;
	}
}

void
object_hit(object_t *o, int dmg, int ap)
{
	int					 ar;
	const rend_entity_t	*ent;
	ent = rend_entity_get(o->ent);

	if (OBJ_HAS(o, OBJ_DESTROYED))
		return;

	ar = RANGE(o->ac, o->ac_max);
	if (ar <= ap) {
		o->hp -= dmg;

		rend_text_append("HIT! %c (\x18%d, \x19%d) DMG \x1E%d",
			ent->c, ap, ar, dmg);
	}
	else {
		rend_text_append("BLOCK! %c (\x18%d, \x19%d)",
			ent->c, ap, ar);
	}

	if (o->hp < 1) {
		switch (o->type) {
		case OBJ_PLAYER:
			object_player_killed(o);
			break;
		case OBJ_DOOR:
			object_door_killed(o);
			break;
		case OBJ_COLUMN:
			object_column_killed(o);
			break;
		case OBJ_NPC_ZOMBIE:
			object_zombie_killed(o);
			break;
		case OBJ_NPC_STATUE:
			object_statue_killed(o);
			break;
		case OBJ_NPC_GOBLIN:
			object_goblin_killed(o);
			break;
		case OBJ_STARTER_CHEST:
		case OBJ_LOOT_CHEST:
		case OBJ_CHEST:
			object_chest_killed(o);
			break;
		default:
			break; /* Does not implement combat. */
		}
	}
}

object_t *
object_create(enum object ot, int x, int y)
{
	size_t new_len;

	if (map_get_at(map, x, y) == MAP_WALL)
		return NULL;

	new_len = map_new_object(map, ot, x, y);
	return map->objects + new_len - 1;
}

int
object_is_wall_at(int x, int y)
{
	/* Simple abstraction for objects to identify walls, 
	   and not just collisions. */
	return map_get_at(map, x, y) == MAP_WALL;
}

int
object_collision_at(int x, int y)
{
	object_t	*o;

	if (object_is_wall_at(x, y))
		return 1;

	o = object_at(x, y);
	if (NULL != o && OBJ_IS(o, OBJ_IS_COLLIDABLE))
		return 1;

	return 0;
}

object_t *
object_first(enum object ot)
{
	size_t	i;

	for (i = 0; i < map->objects_len; i++) {
		if (map->objects[i].enabled && map->objects[i].type == ot)
			return map->objects + i;
	}

	return NULL;
}

object_t *
object_at(int x, int y)
{
	object_t	*o;
	size_t		 i;

	for (i = 0; i < map->objects_len; i++) {
		o = map->objects + i;

		if (o->enabled && o->x == x && o->y == y && !OBJ_IS(o, OBJ_NPC_DEAD)
		 && o->type != OBJ_INSPECTER && o->type != OBJ_SLASH)
			return o;
	}

	return NULL;
}

void
object_drop(object_t *o)
{
	object_t	 dup;
	object_t	*drop;
	SDL_Rect	 off;
	int			 i,
				 o_x;

	/* Add gold if present. */
	if (o->gold > 0 && o->drops_len < 9)
		o->drops[o->drops_len++] = OBJ_GOLD;

	switch (o->drops_len) {
	case 0:
		return;
	case 1:
		off = (SDL_Rect){ 0, 0, 0, 0 };
		break;
	case 2:
	case 3:
		off = (SDL_Rect){ -1, 0, 0, 0 };
		break;
	default:
		off = (SDL_Rect){ -1, -1, 0, 0 };
	}

	off.w = MIN(o->drops_len, 3);
	off.h = o->drops_len / 3 + 1;

	/* Adjust drop size to fit. */
	while (map_get_at(map, o->x, o->y + off.y) == MAP_WALL)
		off.y++;

	while (map_get_at(map, o->x + off.x, o->y) == MAP_WALL)
		off.x++;

	for (i = 0; i < off.w; i++) {
		if (map_get_at(map, o->x + off.x + i, o->y + off.y) == MAP_WALL)
			off.w = i;
	}

	for (i = 0; i < off.h; i++) {
		if (map_get_at(map, o->x + off.x, o->y + off.y + i) == MAP_WALL)
			off.h = i;
	}

#ifdef DEBUG
	debug("Drop zone: (%d,%d)(%d,%d)\n", off.x, off.y, off.w, off.h);
#endif

	o_x = off.x;

	/* Pick the smaller of the 2 values. */
	if ((off.w * off.h) < o->drops_len) {
		warn("Some items were unable to drop.\n", NULL);
		o->drops_len = off.w * off.h;
	}

	memcpy(&dup, o, sizeof(object_t));

	/* Generate and drop all items at the correct adjacent tiles. */
	for (i = 0; i < dup.drops_len; i++) {
		drop = object_create(
			dup.drops[i],
			dup.x + off.x,
			dup.y + off.y
		);

		if (drop->type == OBJ_GOLD && dup.gold > 0)
			drop->gold = dup.gold;

		if ((i + 1) % off.w == 0) {
			off.y++;
			off.x = o_x;
		}
		else
			off.x++;
	}
}

int
object_distance(object_t *o1, object_t *o2)
{
	return sqrt(
		pow(abs(o1->x - o2->x), 2)
	  + pow(abs(o1->y - o2->y), 2)
	);
}

void
object_gc()
{
	size_t		 i,
				 new_objects_len = 0,
				 new_i = 0;
	object_t	*list;

	if (map->objects_len == 0 || NULL == map->objects)
		return;
	
	for (i = 0; i < map->objects_len; i++)
		new_objects_len += map->objects[i].enabled;

	if (new_objects_len > map->objects_len) {
		warn("Objects have invalid enabled values. Cannot GC.\n", NULL);
		return;
	}
	else if(new_objects_len == map->objects_len)
		return; /* Nothing to GC. */

#ifdef DEBUG
	debug("GC: Removing %d object(s).\n", map->objects_len - new_objects_len);
#endif

	list = malloc(sizeof(object_t) * new_objects_len);
	for (i = 0; i < map->objects_len; i++) {
		if (map->objects[i].enabled)
			memcpy(list + new_i++, map->objects + i, sizeof(object_t));
		else
			object_free(map->objects + i);
	}

	free(map->objects);
	map->objects = list;
	map->objects_len = new_objects_len;
}

void
object_free(object_t *o)
{
	switch (o->type) {
	case OBJ_SWORD_LEGEND:
	case OBJ_SWORD_COMMON:
	case OBJ_SWORD_RARE:
	case OBJ_SWORD:
		object_sword_free(o);
		break;
	case OBJ_ARMOR_LEGEND:
	case OBJ_ARMOR_COMMON:
	case OBJ_ARMOR_RARE:
	case OBJ_ARMOR:
		object_armor_free(o);
		break;
	default:
		break;
	}
}

void
object_iterate_restart()
{
	o_iter = 0;
}

object_t *
object_iterate(enum object ot)
{
	object_t	*o;

	while (o_iter < map->objects_len) {
		o = map->objects + o_iter++;

		if (o->enabled && o->type == ot)
			return o;
	}

	return NULL;
}

object_t *
object_iterate_at(int x, int y)
{
	object_t	*o;

	while (o_iter < map->objects_len) {
		o = map->objects + o_iter++;

		if (o->enabled && o->x == x && o->y == y)
			return o;
	}

	return NULL;
}

int
object_get_floor()
{
	return MAX(1, map->floor);
}
