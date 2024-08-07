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

#include <SDL_image.h>

#include "renderer_entity.h"
#include "renderer_util.h"
#include "renderer.h"
#include "log.h"

static rend_t		*renderer;
static SDL_Texture	*_dialog_target;

static void			 _dialog_render_msg(char *);
static SDL_Texture	*_dialog_create_bg(const char *);
static void			 _dialog_render(dialog_t *);

void
rend_dialog_setrend(void *r)
{
	renderer = (rend_t *)r;
}

dialog_t *
rend_dialog_create(char *msg, const char *bg)
{
	dialog_t	*new;

	if (NULL == msg && NULL == bg)
		return NULL;

	new = malloc(sizeof(dialog_t));
	new->message = msg;
	new->background = _dialog_create_bg(bg);

	new->rendered = NULL;
	if (renderer->supports_targets)
		_dialog_render(new);

	return new;
}

void
rend_dialog_free(dialog_t *d)
{
	SDL_DestroyTexture(d->rendered);

	if (d->background)
		SDL_DestroyTexture(d->background);

	free(d);
}

void
rend_dialog_set_msg(dialog_t *diag, char *msg)
{
	diag->message = msg;

	/* Rerender everything. */
	_dialog_render(diag);
}

void
rend_dialog_render(dialog_t *dialog)
{
	if (renderer->supports_targets) {
		if (NULL == dialog->rendered)
			_dialog_render(dialog);

		SDL_RenderCopy(
			renderer->backbuffer,
			dialog->rendered,
			NULL, NULL
		);
	}
	else {
		if (NULL != dialog->background)
			SDL_RenderCopy(renderer->backbuffer, dialog->background, NULL, NULL);

		_dialog_render_msg(dialog->message);
	}
}

static char *
_dialog_render_msg_row(int off_y, size_t max_len, char *msg)
{
	SDL_Rect	 		 chr_src,
				 		 chr_dst;
	size_t		 		 row_len;
	const rend_entity_t	*entity;
	int					 d_r, d_g, d_b;

	d_r = d_g = d_b = 255;

	/* Initialize destination. */
	chr_dst = (SDL_Rect){
		0,
		off_y,
		renderer->cfg->font_x * renderer->cfg->font_scale,
		renderer->cfg->font_y * renderer->cfg->font_scale,
	};

	/* Calculate the length of the row. */
	for (
		row_len = 0;
		row_len < max_len && msg[row_len] && msg[row_len] != '\n';
		row_len++
	);

	if (row_len == 0)
		return msg + 1;

	/* Calculate the initial destination offset. */
	chr_dst.x = (
		((renderer->cfg->win_width) - 
		(renderer->cfg->font_x * renderer->cfg->font_scale * row_len))
		/ 2
	);

	/* Render out the row. */
	do {
		if (0 == *msg)
			break;

		if (*msg == '[')
			d_r = d_g = d_b = 128;
		else if (*(msg - 1) == ']')
			d_r = d_g = d_b = 255;

		if (isspace(*msg))
			msg++;
		else {
			SDL_SetTextureColorMod(renderer->font, d_r, d_g, d_b);

			/* Only color entities that aren't normal characters. */
			if (*msg < ' ' || *msg > '%') {
				entity = rend_entity_for(*msg);

				if (NULL != entity) {
					SDL_SetTextureColorMod(
						renderer->font,
						entity->r,
						entity->g,
						entity->b
					);
				}
			}

			chr_src = rend_get_char(*msg++);

			SDL_RenderCopy(
				renderer->backbuffer,
				renderer->font,
				&chr_src,
				&chr_dst
			);
		}

		chr_dst.x += renderer->cfg->font_x * renderer->cfg->font_scale;
	} while(row_len-- > 0);

	SDL_SetTextureColorMod(renderer->font, 255, 255, 255);
	return msg;
}

static void
_dialog_render_msg(char *msg)
{
	int		y;
	size_t	msg_len,
			max_row_len;

	if (NULL == msg)
		return;

	msg_len = strlen(msg);

	/* Get the maximum allowed characters per row. */
	max_row_len = (
		renderer->cfg->win_width / (
			renderer->cfg->font_x * renderer->cfg->font_scale
		)
	);

	/* Calculate the starting y based on above row length. */
	y = (
		(renderer->cfg->win_height / 2) - 
		(renderer->cfg->font_y * renderer->cfg->font_scale * (
			1 + (msg_len / max_row_len)
		))
	);

	/* Render rows until empty. */
	while (*msg) {
		msg = _dialog_render_msg_row(y, max_row_len, msg);
		y += renderer->cfg->font_y * renderer->cfg->font_scale;
	}

	_dialog_render_msg_row(
		renderer->cfg->win_height - (
			renderer->cfg->font_y * renderer->cfg->font_scale * 2
		),
		max_row_len,
#ifdef VITABUILD
		"Press \xFD to Continue"
#elif __3DS__
		"Press Select to Continue"
#else
		"Press [enter] to Continue"
#endif
	);
}

static SDL_Texture *
_dialog_create_bg(const char *bg_path)
{
	SDL_Surface *bg_surf;

	if (NULL == bg_path)
		return NULL;

	if (NULL == (bg_surf = IMG_Load(bg_path)))
		return NULL;

	return SDL_CreateTextureFromSurface(renderer->backbuffer, bg_surf);
}

static void
_dialog_render(dialog_t *dialog)
{
	int *pixels, pitch, error;

	if (NULL == _dialog_target) {
		/* Create the main rendering target for generating dialogs.
		   The result is then copied over to a static texture for storage. */
		_dialog_target = SDL_CreateTexture(
			renderer->backbuffer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			renderer->cfg->win_width,
			renderer->cfg->win_height
		);

		if (NULL == _dialog_target) {
			warn("Failed to create dialog render target: %s\n", SDL_GetError());
			return;
		}
#ifdef DEBUG
		else 
			debug("Created dialog render target.\n", NULL);
#endif
	}

	/* Render the dialog. */
	if (SDL_SetRenderTarget(renderer->backbuffer, _dialog_target) < 0)
		warn("Unable to set render target: %s\n", SDL_GetError());

	SDL_RenderClear(renderer->backbuffer);

	if (NULL != dialog->background)
		SDL_RenderCopy(renderer->backbuffer, dialog->background, NULL, NULL);

	_dialog_render_msg(dialog->message);

	SDL_RenderPresent(renderer->backbuffer);

	/* Store the frame in a static texture. */
	if (NULL == dialog->rendered) {
		dialog->rendered = SDL_CreateTexture(
			renderer->backbuffer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			renderer->cfg->win_width,
			renderer->cfg->win_height
		);
	}

	if (SDL_LockTexture(dialog->rendered, NULL, (void **)&pixels, &pitch) < 0)
		warn("Unable to lock dialog texture: %s\n", SDL_GetError());

	error = SDL_RenderReadPixels(
		renderer->backbuffer,
		NULL,
		SDL_PIXELFORMAT_RGBA8888,
		pixels, pitch
	);

	if (error < 0)
		warn("Unable to read dialog target pixel data: %s\n", SDL_GetError());

	SDL_UnlockTexture(dialog->rendered);

	SDL_SetRenderTarget(renderer->backbuffer, NULL);
}
