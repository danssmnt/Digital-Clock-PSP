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

#include <pspiofilemgr.h>
#include <pspkernel.h>

#include "utils.h"

/* Check if file exists by checking attributes
 * Better than opening and closing it
 */
cbool file_exists(const char* file_path)
{
  SceIoStat stat = {0};
  
  if ( sceIoGetstat(file_path, &stat) < 0 )
  {
    return FALSE;
  }

  return FIO_SO_ISREG(stat.st_attr);
}