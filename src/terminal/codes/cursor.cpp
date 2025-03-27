#include "terminal/codes/cursor.h"
#include <string>
using std::to_string;

inline std::string cursor_to(int line, int col) {
    if (line < 0 || col < 0) return "";
    return ESC "[" + to_string(line) + ";" + to_string(col) + "H";
}

inline std::string cursor_up(int lines) {
    if (lines < 0) return "";
    return ESC "[" + to_string(lines) + "A";
}

inline std::string cursor_down(int lines) {
    if (lines < 0) return "";
    return ESC "[" + to_string(lines) + "B";
}

inline std::string cursor_right(int cols) {
    if (cols < 0) return "";
    return ESC "[" + to_string(cols) + "C";
}

inline std::string cursor_left(int cols) {
    if (cols < 0) return "";
    return ESC "[" + to_string(cols) + "D";
}

inline std::string cursor_lines_down(int lines) {
    if (lines < 0) return "";
    return ESC "[" + to_string(lines) + "E";
}

inline std::string cursor_lines_up(int lines) {
    if (lines < 0) return "";
    return ESC "[" + to_string(lines) + "F";
}

