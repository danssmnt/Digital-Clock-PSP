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

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <psprtc.h>
#include <psptypes.h>
#include <pspctrl.h>

#include "main.h"
#include "lib/glib2d/glib2d.h"
#include "callbacks.h"
#include "utils.h"
#include "error.h"
#include "tex.h"

#define APP_NAME    "Digital Clock"
#define APP_VERSION "v2.0"

const char* app_name  = APP_NAME;
const char* app_ver   = APP_VERSION;

PSP_MODULE_INFO(APP_NAME, PSP_MODULE_USER, 2, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

cbool app_running = TRUE;

int main(int args, char* argv[])
{
  // -Wextra
  (void)args; (void)argv;

  // Callbacks should never fail
  if ( setup_callbacks() < 0 )
  {
    app_running = FALSE;
    app_error_display(ERROR_SETUP_CALLBACKS);
  }
  
  // Neither should allocating mem for textures
  if ( tex_num_alloc() < 0 )
  {
    app_running = FALSE;
  }

  const g2dColor bg_color = BLACK;

  // Available Clock Colors (L / R to change)
  const g2dColor clock_colors[] = {RED, ORANGE, YELLOW, CHARTREUSE, 
                                   GREEN, SPRING_GREEN, CYAN, AZURE, BLUE, 
                                   MAGENTA, ROSE, VIOLET, WHITE, 
                                  GRAY};
  const int clock_colors_size = ARRAY_SIZE(clock_colors);
  int curr_clock_color_index = 0;

  // Available Brightness Modes (Square to change)
  const uchar brightness_modes[] = { 255, 128, 64 };
  const uchar brightness_modes_size = ARRAY_SIZE(brightness_modes);
  int curr_brightness_index = 0;
  
  const ScePspFVector2 size_sprites = { 80.0f, 160.0f };
  ScePspFVector2 pos_sprites = { 60.0f, (float)G2D_SCR_H / 2.0f };
  
  // Centered
  const ScePspFVector2 colon_pos = { (float)G2D_SCR_W / 2.0f, (float)G2D_SCR_H / 2.0f };
  const ScePspFVector2 colon_size = { 18.0f, 160.0f };

  ScePspDateTime curr_time = {0};
  int old_min;

  SceCtrlLatch latch;

  g2dInit();
  
  while ( app_running )
  {
    // GRAPHICS ///////////////////////////////////////

    pos_sprites.x = 60.0f;
    g2dClear(bg_color);
    
    old_min = curr_time.minute;

    // This should never fail, otherwise something is horribly wrong!
    if ( sceRtcGetCurrentClockLocalTime(&curr_time) < 0 )
    {
      app_running = FALSE;
      app_error_display(ERROR_GETTING_TIME_RTC);
    }
    
    // For checking if min has changed (instead of building the texture array every cycle)
    if ( old_min != curr_time.minute )
    {
      build_num_texdraw(&curr_time);
    }
    
    // Clock display 4 digits (2 for hour and 2 for min)
    for ( int tile = 0; tile < 4; tile++ )
    {
      // Draw if it's not NULL (eg.: first tile is 0)
      if (tile_number_texdraw[tile])
      {
        tex_draw(tile_number_texdraw[tile], &pos_sprites, &size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
      }
      
      pos_sprites.x += 120.0f;
    }
    
    // Draw colon every even second (for blinking)
    if ( curr_time.second % 2 == 0 )
    {
      tex_draw(tex_clock[TEX_SIZE-1], &colon_pos, &colon_size, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
    }

    g2dFlip(G2D_VSYNC);

    // GRAPHICS ///////////////////////////////////////

    // CONTROLS ///////////////////////////////////////

    sceCtrlReadLatch(&latch);

    // Press Select to exit the app
    if ( latch.uiMake & PSP_CTRL_SELECT )
    {
      app_running = FALSE;
    }

    // Press L / R to change color
    if ( latch.uiMake & PSP_CTRL_LTRIGGER )
    {
      curr_clock_color_index = (curr_clock_color_index - 1 + clock_colors_size) % clock_colors_size;
    }
    
    // Press L / R to change color
    else if ( latch.uiMake & PSP_CTRL_RTRIGGER )
    {
      curr_clock_color_index = (curr_clock_color_index + 1) % clock_colors_size;
    }

    // Press Square to change brightness mode
    if ( latch.uiMake & PSP_CTRL_SQUARE )
    {
      curr_brightness_index = (curr_brightness_index + 1) % brightness_modes_size;
    }

    // CONTROLS ///////////////////////////////////////
  }

  g2dTerm();
  tex_free();

  sceKernelExitGame();

  return 0;
}