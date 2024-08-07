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

#ifndef RLONE_AUDIO_H
#define RLONE_AUDIO_H

#include "config.h"

enum music {
	AUD_BGM,
	AUD_MENU,
	AUD_DEATH
};

void	audio_init(cfg_t *);
void	audio_play_bgm();
void	audio_stop_bgm();
void	audio_set_bgm(enum music);
void	audio_free();

#endif
