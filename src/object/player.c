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

#include <string.h>

#include "object_player.h"

/* Objects to interact with. */
#include "object_projectile_arrow.h"
#include "object_projectile.h"
#include "object_slash.h"
#include "object_sword.h"
#include "object_chest.h"
#include "object_door.h"
#include "object.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "renderer.h"
#include "input.h"
#include "map.h"
#include "log.h"

struct player_inventory {
	int 				initialized;
	int					health_potions,
						hp_start,
						ac_start;
	int					ammo, gold;
	enum rend_entity	armor;
	object_t			sword;
	object_t			bow;
};

static dialog_t					*inspect;
static struct player_inventory	 inv;

static int	_player_handle_object(object_t *, object_t *);
static void	_player_update_inspect(object_t *);

void
object_player_init(object_t *p)
{
	p->ent = ENT_PLAYER;

	if (!inv.initialized) {
		inv.armor = ENT_ARMOR_COMMON;
		inv.initialized = 1;

		p->hp = p->hp_max = 1;
		p->ac = p->ac_max = 0;
	}
	else {
		p->hp = p->hp_max = inv.hp_start;
		p->ac = p->ac_max = inv.ac_start;
		p->gold = inv.gold;
		p->ammo = inv.ammo;
	}

	_player_update_inspect(p);
	OBJ_SET(p, OBJ_IS_COLLIDABLE);
}

void
object_player_update(object_t *p)
{
	object_t	*o;
	int			 move = 1,
				 n_x = p->x,
				 n_y = p->y;

	if (OBJ_IS(p, OBJ_PLAYER_DEAD))
		return;

	if (input_clicked(IN_INSPECT)) {
		_player_update_inspect(p);
		object_create(OBJ_INSPECTER, p->x, p->y);
	}
	else if (input_clicked(IN_INSPECT_SWORD) && inv.sword.enabled)
		rend_set_dialog(inv.sword.inspect);
	else if (input_clicked(IN_INSPECT_BOW) && inv.bow.enabled)
		rend_set_dialog(inv.bow.inspect);
	else {
		if (input_is_down(IN_MOVE_UP))
			n_y--;
		if (input_is_down(IN_MOVE_DOWN))
			n_y++;
		if (input_is_down(IN_MOVE_LEFT))
			n_x--;
		if (input_is_down(IN_MOVE_RIGHT))
			n_x++;

		/* On click, attempt to heal player. */
		if (input_clicked(IN_HEAL) && inv.health_potions > 0) {
			inv.health_potions--;
			p->hp = p->hp_max;
		}

		/* Skree. */
		if (input_is_down(IN_SLASH)) {
			if (!OBJ_HAS(p, OBJ_PLAYER_ACTION_SLASHED)) {
				OBJ_SET(p, OBJ_PLAYER_ACTION_SLASH);
			}
			move = 0;
		}
		else {
			OBJ_REM(p, OBJ_PLAYER_ACTION_SLASH);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SLASHED);
		}

		if (input_is_down(IN_SHOOT)) {
			if (!OBJ_HAS(p, OBJ_PLAYER_ACTION_SHOOTED)) {
				OBJ_SET(p, OBJ_PLAYER_ACTION_SHOOT);
			}
			move = 0;
		}
		else {
			OBJ_REM(p, OBJ_PLAYER_ACTION_SHOOT);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SHOOTED);
		}

		if (input_is_down(IN_INTERACT)) {
			if (!OBJ_HAS(p, OBJ_PLAYER_ACTION_INTERACTED))
				OBJ_SET(p, OBJ_PLAYER_ACTION_INTERACT);
			move = 0;
		}
		else {
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACT);
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACTED);
		}

		if (n_y == p->y && n_x == p->x) {
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACTED);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SLASHED);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SHOOTED);
			return;
		}

		if (inv.sword.enabled && OBJ_HAS(p, OBJ_PLAYER_ACTION_SLASH)) {
			/* Ensure the flags are updated before the object buffer is
			   reformed. */
			OBJ_SET(p, OBJ_PLAYER_ACTION_SLASHED);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SLASH);

			object_slash_create(
				n_x, n_y,
				inv.sword.hp, inv.sword.hp_max,
				inv.sword.ac, inv.sword.ac_max
			);
			return;
		}

		if (inv.bow.enabled && OBJ_HAS(p, OBJ_PLAYER_ACTION_SHOOT)) {
			/* Ensure the flags are updated before the object buffer is
			   reformed. */
			OBJ_SET(p, OBJ_PLAYER_ACTION_SHOOTED);
			OBJ_REM(p, OBJ_PLAYER_ACTION_SHOOT);

			if (object_collision_at(n_x, n_y))
				return;

			if (p->ammo > 0) {
				p->ammo--;

				object_projectile_shoot(
					OBJ_PROJECTILE_ARROW,
					n_x, n_y,
					n_x - p->x, n_y - p->y,
					inv.bow.hp, inv.bow.hp_max,
					inv.bow.ac, inv.bow.ac_max
				);
			}
			else
				rend_text_append("Out of ammo!");
			return;
		}

		object_iterate_restart();

		while (NULL != (o = object_iterate_at(n_x, n_y)))
			move = _player_handle_object(p, o) && move;

		if (OBJ_HAS(p, OBJ_PLAYER_ACTION_INTERACT)) {
			OBJ_SET(p, OBJ_PLAYER_ACTION_INTERACTED);
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACT);
		}

		if (move) {
			if (!object_collision_at(p->x, n_y))
				p->y = n_y;
			if (!object_collision_at(n_x, p->y))
				p->x = n_x;
		}

		OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACT);
	}
}

static int
_player_handle_object(object_t *p, object_t *o)
{
	if (o->type == OBJ_STAIRS) {
		OBJ_SET(p, OBJ_PLAYER_NEXT_FLOOR);
		inv.ammo = p->ammo;
		inv.gold = p->gold;
		return 0;
	}

	if (OBJ_HAS(p, OBJ_PLAYER_ACTION_INTERACT)
	&& !OBJ_HAS(p, OBJ_PLAYER_ACTION_INTERACTED)) {
		switch (o->type) {
		case OBJ_PLAYER:
			return 1;
		case OBJ_DOOR:
			object_door_toggle(o);
			return 0;
		case OBJ_CHEST:
		case OBJ_STARTER_CHEST:
		case OBJ_LOOT_CHEST:
			OBJ_SET(p, OBJ_PLAYER_ACTION_INTERACTED);
			object_chest_open(o);
			return 0;
		case OBJ_SWORD_COMMON:
		case OBJ_SWORD_RARE:
		case OBJ_SWORD_LEGEND:
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACT);

			/* If we already have a sword, free the dialog before
			   continuing. */
			if (inv.sword.enabled) {
#ifdef DEBUG
				debug("Replacing sword.\n", NULL);
#endif
				inv.sword.gold = 0;
				object_sword_free(&inv.sword);
			}

			OBJ_SET(p, OBJ_PLAYER_SWORD);
			memcpy(&inv.sword, o, sizeof(object_t));

			rend_text_append("LOOT! \x18 (\x18%d-%d, \x19%d-%d)",
				o->hp, o->hp_max, o->ac, o->ac_max);

			o->gold = 1;
			o->enabled = 0;
			return 0;
		case OBJ_BOW_COMMON:
		case OBJ_BOW_RARE:
		case OBJ_BOW_LEGEND:
			OBJ_REM(p, OBJ_PLAYER_ACTION_INTERACT);

			/* If we already have a sword, free the dialog before
			   continuing. */
			if (inv.bow.enabled) {
#ifdef DEBUG
				debug("Replacing bow.\n", NULL);
#endif
				inv.sword.gold = 0;
				object_sword_free(&inv.bow);
			}

			OBJ_SET(p, OBJ_PLAYER_BOW);
			memcpy(&inv.bow, o, sizeof(object_t));

			rend_text_append("LOOT! \x0C (\x18%d-%d, \x19%d-%d)",
				o->hp, o->hp_max, o->ac, o->ac_max);

			o->gold = 1;
			o->enabled = 0;
			return 0;
		case OBJ_ARMOR_COMMON:
		case OBJ_ARMOR_RARE:
		case OBJ_ARMOR_LEGEND:
			inv.armor = o->ent;
			inv.hp_start = p->hp = p->hp_max = o->hp_max;
			inv.ac_start = p->ac = p->ac_max = o->ac_max;

			rend_text_append("LOOT! \x10 (\x1E%d, \x10%d)",
				o->hp_max, o->ac_max);

			o->enabled = 0;
			return 0;
		case OBJ_HEALTH_POTION:
			inv.health_potions++;
			o->enabled = 0;
			break;
		case OBJ_ARROWS:
			p->ammo += o->ammo;
			rend_text_append("LOOT! \x17%d", o->ammo);
			o->enabled = 0;
			break;
		case OBJ_GOLD:
			p->gold += o->gold;
			rend_text_append("LOOT! \x1D%d", o->gold);
			o->enabled = 0;
			break;
		default:
			return 1;
		}
	}

	return 1;
}

#define INSPECT_FLUFF	"It's you, yourself, and you.\n"
#define INSPECT_DATA	"\x1E%d/%d \x10%d/%d \x1D%d"
#define INSPECT_LEN		(strlen(INSPECT_FLUFF INSPECT_DATA) + 40)

void
_player_update_inspect(object_t *p)
{
	char	*msg;
	int		 msg_len;

	msg = malloc(INSPECT_LEN);
	msg_len = snprintf(msg, INSPECT_LEN, INSPECT_FLUFF INSPECT_DATA,
						p->hp, p->hp_max, p->ac, p->ac_max, p->gold);
	msg[msg_len] = 0;

	if (NULL == inspect) {
		inspect = rend_dialog_create(msg, NULL);
		p->inspect = inspect;
	}
	else {
		free(inspect->message);
		rend_dialog_set_msg(inspect, msg);
	}
}

int	
object_player_get_sword_entity()
{
	if (inv.sword.enabled)
		return inv.sword.ent;
	return ENT_NOTFOUND;
}

int	
object_player_get_bow_entity()
{
	if (inv.bow.enabled)
		return inv.bow.ent;
	return ENT_NOTFOUND;
}

int	
object_player_get_armor_entity()
{
	return inv.armor;
}

int	
object_player_get_health_potions()
{
	return inv.health_potions;
}

void
object_player_killed(object_t *p)
{
	OBJ_SET(p, OBJ_PLAYER_DEAD);
	memset(&inv, 0, sizeof(struct player_inventory));
}
