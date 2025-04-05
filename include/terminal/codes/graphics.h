#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "escape.h"

#define RESET_ALL ESC "[0m"

#define BOLD ESC "[1m"
#define BOLD_RESET ESC "[22m"

// colors
#define FONT_BLUE ESC "[38;5;18m"
#define BG_BLUE ESC "[48;5;18m"

#define FONT_GREEN ESC "[38;5;29m"
#define BG_GREEN ESC "[48;5;29m"

#define FONT_RED ESC "[38;5;88m"
#define BG_RED ESC "[48;5;88m"

#define FONT_LIGHT_BLUE ESC "[38;5;111m"
#define BG_LIGHT_BLUE ESC "[48;5;111m"

#define FONT_PURPLE ESC "[38;5;129m"
#define BG_PURPLE ESC "[48;5;129m"

#define FONT_MAGENTA ESC "[38;5;162m"
#define BG_MAGENTA ESC "[48;5;162m"

#define FONT_ORANGE ESC "[38;5;172m"
#define BG_ORANGE ESC "[48;5;172m"

#define FONT_PINK ESC "[38;5;212m"
#define BG_PINK ESC "[48;5;212m"

#define FONT_YELLOW ESC "[38;5;220m"
#define BG_YELLOW ESC "[48;5;220m"

#define FONT_BLACK ESC "[38;5;234m"
#define BG_BLACK ESC "[48;5;234m"

#define FONT_LIGHT_GRAY ESC "[38;5;247m"
#define BG_LIGHT_GRAY ESC "[48;5;247m"

#endif

