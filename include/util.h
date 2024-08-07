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

#ifndef RLONE_UTIL_H
#define RLONE_UTIL_H

#include "stdlib.h"
#include "limits.h"

#ifdef _WIN32
/* Do whatever Windows decided was good 20 years ago. */
#define INITRAND()	do {\
	unsigned int	seed;\
	rand_s(&seed);\
	srand(seed);\
} while(0)
#elif __OpenBSD__
/* OpenBSD randomness is included in stdlib.h */
#define INITRAND()	srand(arc4random())
#elif __3DS__
#define INITRAND()	srand(rand())
#elif VITABUILD
/* PS Vita random initialization. */
#include <psp2/kernel/rng.h> 
#define INITRAND()	do {\
	unsigned int	seed;\
	sceKernelGetRandomNumber(&seed, sizeof(unsigned int));\
	srand(seed);\
} while(0)
#else
/* If we don't have explicit support defined, use the linux impl. */
#include <sys/random.h>
#define INITRAND()	do {\
	unsigned int	seed;\
	getrandom(&seed, sizeof(unsigned int), 0);\
	srand(seed);\
} while(0)
#endif

/* Normal min/max for general use. */
#define MAX(x, y)	((x) > (y) ? (x) : (y))

/* You wouldn't believe how long into development
   this was implemented wrong. */
#define MIN(x, y)	((x) < (y) ? (x) : (y))

/* Roll an N sided die. */
#define ROLL(N)		((rand() / (float)RAND_MAX) * (N))

/* Math me up, do it. */
#define RANGE(L,H)	((L) + ROLL((H) - (L)))

#endif
