#include <psppower.h>

#include "battery.h"
#include "tex.h"

bat_status bat_get_status()
{
  bat_perc perc = scePowerGetBatteryLifePercent();
  
  if (perc >= 80)
    return BAT_STATUS_FULL;

  else if (perc >= 60)
    return BAT_STATUS_3BARS;

  else if (perc >= 30)
    return BAT_STATUS_2BARS;

  else if (perc >= 10)
    return BAT_STATUS_1BARS;
  
  return BAT_STATUS_EMPTY;
}

void get_tex_by_curr_bat_status(app_tex** out)
{
  switch(bat_get_status())
    {
      case BAT_STATUS_FULL:
        *out = &main_clock_tex.s.bat_full;
        break;

      case BAT_STATUS_3BARS:
        *out = &main_clock_tex.s.bat_3bar;
        break;

      case BAT_STATUS_2BARS:
        *out = &main_clock_tex.s.bat_2bar;
        break;

      case BAT_STATUS_1BARS:
        *out = &main_clock_tex.s.bat_1bar;
        break;

      case BAT_STATUS_EMPTY:
      default:
        *out = &main_clock_tex.s.bat_empty;
        break;
    }
}