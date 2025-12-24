#pragma once

struct ClockTime {
    int hours;
    int minutes;
    int seconds;
};

ClockTime getLocalClockTime();
