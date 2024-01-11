#include "../modules/px_time.h"
#include "time.h"
#include "gui_api.h"

unsigned int PX_TimeGetTime()
{
    return gui_ms_get();
}

int PX_TimeGetYear()
{
    return 2024;
}

int PX_TimeGetMouth()
{
    return 1;
}

int PX_TimeGetDay()
{
    return 11;
}

int PX_TimeGetHour()
{
    return 9;
}

int PX_TimeGetMinute()
{
    return 44;
}

int PX_TimeGetSecond()
{
    return 50;
}

void PX_Sleep(unsigned int ms)
{
    gui_thread_mdelay(ms);
}
