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

#ifndef ERROR_H_
#define ERROR_H_

#include "utils.h"

typedef enum
{
  ERROR_SETUP_CALLBACKS,
  ERROR_TEXTURES_NOT_FOUND,
  ERROR_ALLOCATING_TEXTURES,
  ERROR_GETTING_TIME_RTC,
  ERROR_UNKNOWN
} app_error_type;

typedef struct
{
  uint id;
  char* name;
  char* msg;
} app_error;

extern int app_error_display(app_error_type err);

#endif /* ERROR_H_ */