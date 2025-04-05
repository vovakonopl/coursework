#include "terminal/codes/cursor.h"
#include <string>
using std::to_string;
using std::string;

string cursor_to(int line, int col) {
    if (line < 0 || col < 0) return "";
    return ESC "[" + to_string(line) + ";" + to_string(col) + "H";
}

string cursor_up(int lines) {
    if (lines <= 0) return "";
    return ESC "[" + to_string(lines) + "A";
}

string cursor_down(int lines) {
    if (lines <= 0) return "";
    return ESC "[" + to_string(lines) + "B";
}

string cursor_right(int cols) {
    if (cols <= 0) return "";
    return ESC "[" + to_string(cols) + "C";
}

string cursor_left(int cols) {
    if (cols <= 0) return "";
    return ESC "[" + to_string(cols) + "D";
}

string cursor_lines_down(int lines) {
    if (lines <= 0) return "";
    return ESC "[" + to_string(lines) + "E";
}

string cursor_lines_up(int lines) {
    if (lines <= 0) return "";
    return ESC "[" + to_string(lines) + "F";
}
