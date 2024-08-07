#include "object_gold.h"

#include "renderer_entity.h"
#include "renderer_dialog.h"
#include "util.h"

static dialog_t	*inspect;

void
object_gold_init(object_t *o)
{
	int	floor = object_get_floor();

	o->ent = ENT_COIN;
	o->gold = RANGE(1, 20 * (floor + 1));

	if (NULL == inspect) {
		inspect = rend_dialog_create(
			"An indeterminate amount of coins.", NULL
		);
	}

	o->inspect = inspect;
}
