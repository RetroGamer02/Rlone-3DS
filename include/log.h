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

#ifndef RLONE_LOG_H
#define RLONE_LOG_H

#include <stdlib.h>
#include <stdio.h>

#ifdef VITABUILD
#include <psp2/kernel/processmgr.h>
#endif

#ifdef __3DS__
#include <string.h>
#include <ctype.h>
#endif

enum log_level {
	LOG_INFO,
	LOG_WARN,
	LOG_FATAL,
#ifdef DEBUG
	LOG_DEBUG,
#endif
};

void	 log_init(const char *);
void	 log_close();
int		 log_log(enum log_level, const char *, ...);

#ifdef DEBUG
#define debug(fmt, ...)	log_log(LOG_DEBUG, fmt, __VA_ARGS__)
#endif

#define info(fmt, ...)	log_log(LOG_INFO, fmt, __VA_ARGS__)
#define warn(fmt, ...)	log_log(LOG_WARN, fmt, __VA_ARGS__)
#ifdef VITABUILD
#define fatal(fmt, ...) do {\
	sceKernelExitProcess(log_log(LOG_FATAL, fmt, __VA_ARGS__));\
	exit(0);\
} while(0)
#else
#define fatal(fmt, ...)	exit(log_log(LOG_FATAL, fmt, __VA_ARGS__))
#endif

#endif
