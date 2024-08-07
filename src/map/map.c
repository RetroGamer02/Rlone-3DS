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

#include "util.h"
#include "map.h"
#include "log.h"

void
map_init_rand()
{
	INITRAND();
	

#ifdef DEBUG
	debug("RNG has been seeded via urandom.\n", NULL);
#endif
}

char
map_get_at(map_t *m, int x, int y)
{
	return m->buffer[(y * m->size.w) + x];
}

char
map_set_at(map_t *m, int x, int y, char v)
{
	char	old;

	old = map_get_at(m, x, y);
	m->buffer[(y * m->size.w) + x] = v;

	return old;
}

object_t *
_map_find_empty(map_t *m)
{
	size_t	i;

	for (i = 0; i < m->objects_len; i++) {
		if (!m->objects[i].enabled)
			return m->objects + i;
	}

	return NULL;
}

size_t
map_new_object(map_t *m, enum object ot, int x, int y)
{
	object_t	*list, *empty;
	size_t		 old_len = m->objects_len;

	empty = _map_find_empty(m);

	if (NULL == empty) {
		list = malloc(sizeof(object_t) * ++m->objects_len);
		if (NULL != m->objects) {
			memcpy(list, m->objects, sizeof(object_t) * old_len);
			free(m->objects);
		}

		m->objects = list;
	}
	else {
		old_len = empty - m->objects;
		object_free(empty);
	}

	/* Zero the new memory. */
	memset(m->objects + old_len, 0, sizeof(object_t));

	/* Set the new objects type */
	(m->objects + old_len)->type = ot;

	/* Set the objects position. */
	(m->objects + old_len)->x = x;
	(m->objects + old_len)->y = y;

	/* Initialize the new object. */
	object_init(m->objects + old_len);

	/* Set the objects position. */
	(m->objects + old_len)->x = x;
	(m->objects + old_len)->y = y;

#ifdef DEBUG
	debug("Created object %d at (%d,%d).\n",
		(m->objects + old_len)->type,
		(m->objects + old_len)->x,
		(m->objects + old_len)->y);
#endif

	if (empty != NULL)
		return old_len + 1;

	return m->objects_len;
}

void
map_free(map_t *m)
{
	size_t i;

	if (m->objects != NULL) {
		for (i = 0; i < m->objects_len; i++)
			object_free(m->objects + i);
		free(m->objects);
	}

	free(m);
}
