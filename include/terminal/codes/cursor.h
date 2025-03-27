#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include "escape.h"

#define CURSOR_SAVE ESC "[s"
#define CURSOR_RESTORE ESC "[u"
#define REQUEST_CURSOR_POS ESC "[6n" // responds with "ESC[Y;XR" (y - line, x - col)
#define CURSOR_HOME ESC "[H" // home position - 0, 0
 
inline std::string cursor_to(int line, int col);
inline std::string cursor_up(int lines);
inline std::string cursor_down(int lines);
inline std::string cursor_right(int cols);
inline std::string cursor_left(int cols);

// next utils moves cursor to the start of the line
#define CURSOR_NEXT_LINE ESC "[1E"
#define CURSOR_PREV_LINE ESC "[1F"

inline std::string cursor_lines_down(int lines);
inline std::string cursor_lines_up(int lines); 

#endif
