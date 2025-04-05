#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include "escape.h"

#define CURSOR_SAVE ESC "[s"
#define CURSOR_RESTORE ESC "[u"
#define REQUEST_CURSOR_POS ESC "[6n" // responds with "ESC[Y;XR" (y - line, x - col)
#define CURSOR_HOME ESC "[H" // home position - 0, 0
#define CURSOR_INVISIBLE ESC "[?25l" 
#define CURSOR_VISIBLE ESC "[?25h" 

std::string cursor_to(int line, int col);
std::string cursor_up(int lines);
std::string cursor_down(int lines);
std::string cursor_right(int cols);
std::string cursor_left(int cols);

// next utils moves cursor to the start of the line
#define CURSOR_NEXT_LINE ESC "[1E"
#define CURSOR_PREV_LINE ESC "[1F"

std::string cursor_lines_down(int lines);
std::string cursor_lines_up(int lines); 

#endif
