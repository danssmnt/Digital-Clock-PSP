/*
 *  Digital Clock for PSP
 *
 *  Copyright (C) 2025, danssmnt
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEX_H_
#define TEX_H_

#include <psprtc.h>

#include "lib/glib2d/glib2d.h"

#define TEX_SIZE 11

extern g2dTexture* tex_clock[TEX_SIZE];
extern g2dTexture* tile_number_texdraw[4];

extern int tex_num_alloc(void);
extern int tex_free(void);
extern int build_num_texdraw(const ScePspDateTime* time);
extern int tex_draw(g2dTexture* tex, const ScePspFVector2* pos, const ScePspFVector2* size, g2dColor color);

#endif /* TEX_H_ */