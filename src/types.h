#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "pd_api.h"

typedef int32_t fixed16x16;

typedef void(*State)();
#define STATE_RUN(S) if (S) S()

