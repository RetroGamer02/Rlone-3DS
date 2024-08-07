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

#ifndef RLONE_GAME_OBJECT_H
#define RLONE_GAME_OBJECT_H

#include <stdint.h>

#ifdef __3DS__
#include <math.h>
#endif

#include "renderer_entity.h"
#include "renderer_dialog.h"

enum object {
	OBJ_PLAYER,
	OBJ_MANAGER,
	OBJ_INSPECTER,
	OBJ_DOOR,
	OBJ_CHEST,
	OBJ_STARTER_CHEST,
	OBJ_LOOT_CHEST,
	OBJ_SWORD_LEGEND,
	OBJ_SWORD_COMMON,
	OBJ_SWORD_RARE,
	OBJ_SWORD,
	OBJ_BOW_LEGEND,
	OBJ_BOW_COMMON,
	OBJ_BOW_RARE,
	OBJ_BOW,
	OBJ_ARMOR_LEGEND,
	OBJ_ARMOR_COMMON,
	OBJ_ARMOR_RARE,
	OBJ_ARMOR,
	OBJ_SLASH,
	OBJ_HEALTH_POTION,
	OBJ_COLUMN,
	OBJ_TICKER,
	OBJ_ARROWS,
	OBJ_ARROWS_LOTS,
	OBJ_PROJECTILE_ARROW,
	OBJ_PROJECTILE_ARROW_PATHED,
	OBJ_NPC_ZOMBIE,
	OBJ_NPC_STATUE,
	OBJ_NPC_GOBLIN,
	OBJ_GOLD,
	OBJ_LOOT,
	OBJ_MOB,
	OBJ_STAIRS,
#ifdef DEBUG
	OBJ_NPC_DEBUG
#endif
};

/* Tier weight struct. */
typedef struct {
	enum object	type;
	int			weight;
} tier_weight_t;

#define OBJ_REND_DISABLED	(0x1)

#define OBJ_REND_HAS(o, f)	(((o)->rend_flags & (f)) ? 1 : 0)

#define OBJ_IS_COLLIDABLE	(0x1)
#define OBJ_DESTROYED		((uint64_t)0x1 << 63)

/* Easy flag checker. */
#define OBJ_IS(o, f)		((((o)->flags & (f)) > 0) ? 1 : 0)
#define OBJ_HAS				OBJ_IS
#define OBJ_SET(o, f)		((o)->flags |= (f))
#define OBJ_REM(o, f)		((o)->flags = (o)->flags ^ ((o)->flags & (f)))

#define OBJ_MAX_DROPS		(9)

typedef struct {
	int	enabled;

	/* IDs */
	enum object			type;
	enum rend_entity	ent;

	/* Render-related flags, not for update overriding. */
	uint64_t	 rend_flags;

	/* General-purpose flags. */
	uint64_t	 flags;

	/* Location */
	int			 x, y;

	/* Update speed accumulator */
	float		 us_a;

	/* Health, AC, etc. */
	int16_t		 hp, ac,
				 hp_max, ac_max;

	/* Attack rolls, NPC only. */
	int16_t		 dmg, dmg_max,
				 ap, ap_max;
	
	int			 ammo;
	int			 gold;

	/* Sub-objects, for loot drops. */
	int8_t		 drops[OBJ_MAX_DROPS],
				 drops_len;

	/* Inspect dialog. */
	dialog_t	*inspect;
} object_t;

object_t	 *object_first(enum object);

/* Static iterator. */
void		  object_iterate_restart();
object_t	 *object_iterate(enum object);
object_t	 *object_iterate_at(int, int);

/* Collision and helper functions. */
object_t	 *object_at(int, int);
int			  object_is_wall_at(int, int);
int			  object_collision_at(int, int);

object_t	 *object_create(enum object, int, int);
void		  object_init(object_t *);
void		  object_update(object_t *, float);
void		  object_hit(object_t *, int, int);
void		  object_drop(object_t *);
int			  object_distance(object_t *, object_t *);
int			  object_get_floor();

/* For managing collisions and helper functions. */
void		  object_setmap(void *);

/* Object GC. Removes any disabled objects. Do not call per frame. */
void		  object_free(object_t *);
void		  object_gc();

#endif
