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

#include "utils.h"
#include "main.h"

// Will execute when user tries to exit app
static int exit_callback(int arg1, int arg2, void *common)
{
  // -Wextra
  (void)arg1; (void)arg2; (void)common;

  // While loop on main() will end, sceKernelExitGame() is called after freeing mem and terminating glib2d
  app_running = FALSE;
  return 0;
}

// Callback Thread
static int callback_thread(SceSize args, void *argp)
{
  // -Wextra
  (void)args; (void)argp;

  int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);

  if ( cbid < 0 )
  {
    app_running = FALSE;
    return -1;
  }

  if ( sceKernelRegisterExitCallback(cbid) < 0 )
  {
    app_running = FALSE;
    return -1;
  }

  sceKernelSleepThreadCB();
  return 0;
}

// Setup Callbacks for exiting app
int setup_callbacks(void)
{
  SceUID thid = sceKernelCreateThread("Callback Update Thread", callback_thread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, NULL);
  
  if( thid < 0 )
  {
    return -1;
  }
  
  if ( sceKernelStartThread(thid, 0, NULL) < 0 )
  {
    return -1;
  }

  return thid;
}