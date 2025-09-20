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
#include <stdio.h>
#include <psptypes.h>
#include <pspctrl.h>

#include "main.h"
#include "lib/glib2d/glib2d.h"
#include "callbacks.h"

#define APP_NAME    "Digital Clock"
#define APP_VERSION "v2.0"

PSP_MODULE_INFO(APP_NAME, PSP_MODULE_USER, 2, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

ubool app_running = TRUE;

#define TEX_SIZE 11
static g2dTexture* tex_clock[TEX_SIZE];

// Textures drawn on screen
static g2dTexture* tile_number_texdraw[4];

// Checks if file exists by opening it
static ubool file_exists(const char* file_path)
{
  SceUID fd = sceIoOpen(file_path, PSP_O_RDONLY, 0777);

  if( fd >= 0 )
  {
    sceIoClose(fd);
    return TRUE;
  }

  return FALSE;
}

// Errors
static const app_error app_errors[] = 
{
  {0x80000000 | ERROR_SETUP_CALLBACKS,      "ERROR_SETUP_CALLBACKS",      "Something went wrong while configuring the Exit Callback."},
  {0x80000000 | ERROR_TEXTURES_NOT_FOUND,   "ERROR_TEXTURES_NOT_FOUND",   "Textures are missing from 'assets/textures/'!\n  Make sure all PNGs are in the correct directory."},
  {0x80000000 | ERROR_ALLOCATING_TEXTURES,  "ERROR_ALLOCATING_TEXTURES",  "Unable to allocate memory for textures."},
  {0x80000000 | ERROR_GETTING_TIME_RTC,     "ERROR_GETTING_TIME_RTC",     "sceRtcGetCurrentClockLocalTime() failed to provide time."},
  {0x7FFFFFFF,                              "ERROR_UNKNOWN",              "Something unknown went very wrong."},
};

// Display error to screen (aborts app...)
static int err_display(error_type err)
{
  // Clear stuff
  g2dTerm();
  (void)tex_free();

  ubool error_running = TRUE;

  const app_error* error = &app_errors[err];

  pspDebugScreenInit();

  SceCtrlLatch err_latch;

  while ( error_running )
  {
    // Print error
    pspDebugScreenClear();

    pspDebugScreenSetXY(2, 2);
    pspDebugScreenSetTextColor(YELLOW);
    pspDebugScreenPrintf("%s %s by danssmnt", APP_NAME, APP_VERSION);

    pspDebugScreenSetXY(2, 5);
    pspDebugScreenSetTextColor(RED);
    pspDebugScreenPrintf("An error occurred");

    pspDebugScreenSetXY(2, 9);
    pspDebugScreenSetTextColor(WHITE);
    pspDebugScreenPrintf("Error Name:        %s", error->name);

    pspDebugScreenSetXY(2, 11);
    pspDebugScreenPrintf("Error Code:        0x%08X", error->id);

    pspDebugScreenSetXY(2, 15);
    pspDebugScreenPrintf("Error Description: %s", error->msg);

    pspDebugScreenSetXY(2, 31);
    pspDebugScreenPrintf("Press SELECT to exit the app.");

    (void)sceDisplayWaitVblankStart();

    (void)sceCtrlReadLatch(&err_latch);

    // Press Select to exit the app
    if ( err_latch.uiMake & PSP_CTRL_SELECT )
    {
      error_running = FALSE;
      continue;
    }
  }

  sceKernelExitGame();

  return 0;
}

// Allocate digital number textures
static int tex_alloc(void)
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
      err_display(ERROR_TEXTURES_NOT_FOUND);
      return -1;
    }

    tex_clock[i] = g2dTexLoad(path_buffer, G2D_SWIZZLE);

    // If failed to alloc, stop
    if ( !tex_clock[i] )
    {
      err_display(ERROR_ALLOCATING_TEXTURES);
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

// Builds the drawn textures array from pspTime (hours & minutes)
static int build_num_texdraw(const pspTime* time)
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
  int m_1digit = (time->minutes / 10);
  int m_2digit = (time->minutes % 10);

  // Organize tiles
  tile_number_texdraw[0] = tex_clock[h_1digit];
  tile_number_texdraw[1] = tex_clock[h_2digit];
  tile_number_texdraw[2] = tex_clock[m_1digit];
  tile_number_texdraw[3] = tex_clock[m_2digit];

  return 0;
}

// Draw textures on screen (with glib2d)
static int tex_draw(g2dTexture* tex, const ScePspFVector2* pos, const ScePspFVector2* size, g2dColor color)
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

int main(int argc, char* argv[])
{
  // -Wextra
  (void)argc; (void)argv;

  // Callbacks shouldn't never fail
  if ( setup_callbacks() < 0 )
  {
    app_running = FALSE;
    err_display(ERROR_SETUP_CALLBACKS);
  }
  // Neither allocating mem for textures
  if ( tex_alloc() < 0 )
  {
    app_running = FALSE;
  }

  const g2dColor bg_color = BLACK;

  // Available Clock Colors (L / R to change)
  const g2dColor clock_colors[] = {RED, ORANGE, YELLOW, GREEN, 
                                   CYAN, BLUE, MAGENTA, VIOLET, 
                                   WHITE, GRAY};
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

  pspTime curr_time = {0};
  int old_min;

  SceCtrlLatch latch;

  g2dInit();
  
  while ( app_running )
  {
    // GRAPHICS ///////////////////////////////////////

    pos_sprites.x = 60.0f;
    g2dClear(bg_color);
    
    old_min = curr_time.minutes;

    // This should never fail, otherwise something is horribly wrong!
    if ( sceRtcGetCurrentClockLocalTime(&curr_time) < 0 )
    {
      app_running = FALSE;
      err_display(ERROR_GETTING_TIME_RTC);
    }
    
    // For checking if min has changed (instead of building the texture array every cycle)
    if ( old_min != curr_time.minutes )
    {
      (void)build_num_texdraw(&curr_time);
    }
    
    // Clock display 4 digits (2 for hour and 2 for min)
    for ( int tile = 0; tile < 4; tile++ )
    {
      (void)tex_draw(tile_number_texdraw[tile], &pos_sprites, &size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
      
      pos_sprites.x += 120.0f;
    }
    
    // Draw colon every even second (for blinking)
    if ( curr_time.seconds % 2 == 0 )
    {
      (void)tex_draw(tex_clock[TEX_SIZE-1], &colon_pos, &colon_size, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
    }

    g2dFlip(G2D_VSYNC);

    // GRAPHICS ///////////////////////////////////////

    // CONTROLS ///////////////////////////////////////

    (void)sceCtrlReadLatch(&latch);

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
  
  (void)tex_free();
  sceKernelExitGame();

  return 0;
}