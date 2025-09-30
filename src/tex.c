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

#include <stdio.h>
#include <psprtc.h>

#include "lib/glib2d/glib2d.h"
#include "utils.h"
#include "error.h"
#include "tex.h"

// All needed textures
g2dTexture* tex_clock[TEX_SIZE];

// Textures drawn on screen
g2dTexture* tile_number_texdraw[4];

// Allocate digital number textures
int tex_num_alloc(void)
{
  // Filenames of textures which will be loaded
  static const char* texs_to_load[TEX_SIZE] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "colon"};

  char path_buffer[32];

  for ( int i = 0; i < TEX_SIZE; i++ )
  {
    snprintf(path_buffer, sizeof(path_buffer), "assets/textures/%s.png", texs_to_load[i]);

    // Check if texture file exists
    if ( !file_exists(path_buffer) )
    {
      app_error_display(ERROR_TEXTURES_NOT_FOUND);
      return -1;
    }

    tex_clock[i] = g2dTexLoad(path_buffer, G2D_SWIZZLE);

    // If failed to alloc, stop
    if ( !tex_clock[i] )
    {
      app_error_display(ERROR_ALLOCATING_TEXTURES);
      return -1;
    }
  }

  return 0;
}

// Frees allocated mem for textures
int tex_free(void)
{
  for ( int i = 0; i < TEX_SIZE; i++ )
  {
    g2dTexFree(&tex_clock[i]);
  }

  return 0;
}

// Builds the drawn textures array from ScePspDateTime (hours & minutes)
int build_num_texdraw(const ScePspDateTime* time)
{
  // This shouldn't fail, if it fails, nothing will be drawn
  if ( !time )
  {
    return -1;
  }

  // Get first and second digits of integer
  int h_1digit = (time->hour / 10);
  int h_2digit = (time->hour % 10);

  // Get first and second digits of integer
  int m_1digit = (time->minute / 10);
  int m_2digit = (time->minute % 10);

  // Organize tiles, if first tile is 0, don't draw
  tile_number_texdraw[0] = h_1digit != 0 ? tex_clock[h_1digit] : NULL;
  tile_number_texdraw[1] = tex_clock[h_2digit];
  tile_number_texdraw[2] = tex_clock[m_1digit];
  tile_number_texdraw[3] = tex_clock[m_2digit];

  return 0;
}

// Draw textures on screen (with glib2d)
int tex_draw(g2dTexture* tex, const ScePspFVector2* pos, const ScePspFVector2* size, g2dColor color)
{
  if ( !tex || !pos || !size || G2D_GET_A(color) == 0)
  {
    return -1;
  }
  
  // Render texture on screen with params
  g2dBeginRects(tex);
  g2dSetCoordMode(G2D_CENTER);
  g2dSetCoordXY(pos->x, pos->y);
  g2dSetScaleWH(size->x, size->y);
  g2dSetColor(color);
  g2dAdd();
  g2dEnd();

  return 0;
}