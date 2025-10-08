#ifndef BATTERY_H_
#define BATTERY_H_

#include "utils.h"
#include "tex.h"

typedef int bat_perc;
typedef uchar bat_status;

enum
{
    BAT_STATUS_FULL,
    BAT_STATUS_3BARS,
    BAT_STATUS_2BARS,
    BAT_STATUS_1BARS,
    BAT_STATUS_EMPTY,
};

bat_status bat_get_status();
void get_tex_by_curr_bat_status(app_tex** out);

#endif /* BATTERY_H_ */