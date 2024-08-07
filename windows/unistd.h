#pragma once

/* Header file to mock whatever unistd functionality is used in iniparser for Windows. */

#include <stdlib.h>

#ifdef _WIN64
typedef signed long long ssize_t;
#else
typedef signed int ssize_t;
#endif
