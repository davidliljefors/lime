#pragma once

#include "Core.h"

struct Timer
{
    i64 start;
    i64 freq;
};

void timer_init(Timer* timer);

void timer_start(Timer* timer);

double timer_elapsed_ms(Timer* timer);