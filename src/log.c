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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "log.h"

static FILE		**_outputs;
static size_t	  _num_outputs;

const char	*lvl_names[] = {
	[LOG_INFO]  = "INFO",
	[LOG_WARN]  = "WARN",
	[LOG_FATAL] = "FATAL",
#ifdef DEBUG
	[LOG_DEBUG] = "DEBUG"
#endif
};

void
log_init(const char *fn)
{
	_num_outputs = 1;
	if (NULL != fn)
		_num_outputs++;

	_outputs = malloc(sizeof(FILE *) * _num_outputs);

	/* Open the respective outputs. */
	_outputs[0] = stderr;
	if (NULL != fn)
		_outputs[1] = fopen(fn, "w+");
}

void
log_close()
{
	if (_num_outputs > 1)
		fclose(_outputs[1]);

	free(_outputs);
	_outputs = NULL;
}

int
log_log(enum log_level lvl, const char *fmt, ...)
{
	va_list	 		 va;
	unsigned int	 i;
	char			*dt;
	time_t			 curtime = time(NULL);


	if (NULL == _outputs)
		return -1;

	dt = asctime(localtime(&curtime));
	dt[strlen(dt) - 1] = 0; /* Trim the newline. */

	va_start(va, fmt);
	for (i = 0; i < _num_outputs; i++) {
		fprintf(_outputs[i], "[%s] %s: ", dt, lvl_names[lvl]);
		vfprintf(_outputs[i], fmt, va);
	}
	va_end(va);

	return lvl;
}
