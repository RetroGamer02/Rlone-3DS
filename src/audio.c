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

#include <SDL_mixer.h>

#include "audio.h"
#include "log.h"

static Mix_Music	*bgm,
					*menu,
					*death,
					*current;

void
audio_init(cfg_t *cfg)
{
	if (-1 == Mix_OpenAudio(cfg->sample_rate, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)) {
		warn("Audio was unable to initialize: %s\n", Mix_GetError());
		return;
	}

	if (NULL == (bgm = Mix_LoadMUS(cfg->bgm_path)))
		warn("Unable to load BGM: %s\n", Mix_GetError());

	if (NULL == (menu = Mix_LoadMUS(cfg->menu_path)))
		warn("Unable to load menu music: %s\n", Mix_GetError());

	if (NULL == (death = Mix_LoadMUS(cfg->death_path)))
		warn("Unable to load death music: %s\n", Mix_GetError());

	Mix_VolumeMusic(cfg->volume * MIX_MAX_VOLUME);
}

void
audio_play_bgm()
{
	int was_playing = Mix_PlayingMusic();

	if (NULL != current) {
		if (!was_playing)
			Mix_FadeInMusic(current, -1, 1000);
		else
			Mix_PlayMusic(current, current == death ? 0 : -1);
	}
}

void
audio_stop_bgm()
{
	Mix_HaltMusic();
}

void
audio_set_bgm(enum music choice)
{
	int			 was_playing = Mix_PlayingMusic();
	Mix_Music	*new;

	switch (choice) {
	case AUD_BGM:
		new = bgm;
		break;
	case AUD_MENU:
		new = menu;
		break;
	case AUD_DEATH:
		new = death;
		break;
	default:
		return;
	}

	if (new == current)
		return;

	current = new;

#ifdef DEBUG
	debug("BGM set to %d.\n", choice);
#endif

	if (was_playing)
		audio_play_bgm();
}

void
audio_free()
{
	if (NULL != bgm)
		Mix_FreeMusic(bgm);

	Mix_CloseAudio();
}
