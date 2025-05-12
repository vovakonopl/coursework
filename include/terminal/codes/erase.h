#ifndef ERASE_H
#define ERASE_H

#include "escape.h"

#define ERASE_CURSOR_TO_LINE_END ESC "[0K"
#define ERASE_CURSOR_TO_LINE_START ESC "[1K"
#define ERASE_CURSOR_TO_SCREEN_END ESC "[0J"
#define ERASE_LINE ESC "[2K"
#define ERASE_SCREEN ESC "[2J"

#endif

