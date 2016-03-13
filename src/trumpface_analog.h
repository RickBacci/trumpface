#pragma once
#include "pebble.h"

# define NUM_CLOCK_TICKS 11

static const GPathInfo MINUTE_HAND_POINTS = {
  6, (GPoint []) {
    { 0, -25 },
    { -4, 2 },
    { 0, 5 },
    { 4, 2 },
    { 0, -25 },
    { 0, 5 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  6, (GPoint []){
    { 0, -15 },
    { -4, 2 },
    { 0, 5 },
    { 4, 2 },
    { 0, -15 },
    { 0, 5 }
  }
};
