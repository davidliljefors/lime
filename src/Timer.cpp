#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

void timer_init(Timer* timer) { 
	QueryPerformanceFrequency((LARGE_INTEGER*)&timer->freq); 
}

void timer_start(Timer* timer)
{
    LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	timer->start = li.QuadPart;
}

double timer_elapsed_ms(Timer* timer)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (li.QuadPart - timer->start) * 1000.0 / timer->freq;
}