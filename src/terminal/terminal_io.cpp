#include "terminal/terminal_io.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/graphics.h"
#include <iostream>
#include <limits>

using std::cout;
using std::cin;
using std::string;
using std::string_view;

void Terminal::clear_input() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Terminal::clear_terminal() {
    cout << ERASE_SCREEN CURSOR_HOME;
}

ArrowKey Terminal::read_arrow_key() {
    char ch = cin.get();
    if (ch != ESC_CHAR) {
        cin.putback(ch);
        return ArrowKey::None;
    } 

    ch = cin.get();
    if (ch != '[') {
        cin.putback(ch);
        cin.putback(ESC_CHAR);
        return ArrowKey::None;
    } 

    ch = cin.get();
    switch (ch) {
        case 'A': 
            return ArrowKey::Up;

        case 'B': 
            return ArrowKey::Down;
        
        case 'c': 
            return ArrowKey::Right;

        case 'D': 
            return ArrowKey::Left;

        default:
            cin.putback(ch);
            cin.putback('[');
            cin.putback(ESC_CHAR);
            return ArrowKey::None;
    }
}

void Terminal::cursor_to_cell(
    Board &board,
    int cell_width,
    int cell_height,
    int row,
    int col
) {
    if (row < 0) {
        row = 0;
    }
    if (col < 0) {
        col = 0;
    }
    row = std::min(row, board.get_rows());
    col = std::min(col, board.get_cols());

    // +1 for the border line
    int line = 1 * BORDER_CHAR_LEN + row * (cell_height + 1);
    int column = 1 * BORDER_CHAR_LEN + col * (cell_width + 1);
    cout << cursor_to(line, column);
}

Board Terminal::read_board() {
    const string msg = "Enter the size of the board (n x n). n = "; 
    int board_size;
    
    clear_terminal();
    cout << msg;
    bool has_size = false;
    while (!has_size) {
        cin >> board_size;
        if (cin.fail()) {
            cin.clear();

            bool is_arrow_key = true;
            while (is_arrow_key) {
                is_arrow_key = read_arrow_key() != ArrowKey::None;
            }

            cin >> board_size;
            if (cin.fail()) {
                cin.clear();
                clear_input();
                clear_terminal();
                cout << "Invalid input! " << msg;
                continue;
            }
        }

        has_size = true;
    }

    clear_input();
    return Board(board_size, board_size);
}

// outputs a full border line (whole row)
// takes characters in params to output them on specified places 
void print_border_line(
    int cols,
    int cell_width,
    string_view main_char, // fills main space (e.g. ═══)
    string_view first_char, // char at the beginning of the line
    string_view split_char, // char at the place where the borders may somehow cross
    string_view last_char // char at the end of the line
) {
    cout << first_char;
    for (int col = 0; col < cols; col++) {
        for (int i = 0; i < cell_width; i++) {
            cout << main_char;
        }

        if (col != cols - 1) {
            cout << split_char; 
        }
    }
    cout << last_char << std::endl;
}

void Terminal::render_board(Board &board) {
    const int cell_width = 3;
    const int cell_height = 1;
    const int rows = board.get_rows();
    const int cols = board.get_cols();
    clear_terminal();
    cout << FONT_LIGHT_GRAY;

    // first line
    print_border_line(cols, cell_width, HORIZ_BORDER, TL_CORNER, BH_BORDER, TR_CORNER);

    // main content
    for (int row = 0; row < rows; row++) {
        for (int i = 0; i < cell_height; i++) {
            print_border_line(cols, cell_width, " ", VERT_BORDER, VERT_BORDER, VERT_BORDER);
        }

        if (row != rows - 1) {
            print_border_line(cols, cell_width, HORIZ_BORDER, RV_BORDER, CROSSED_BORDERS, LV_BORDER);
        }
    }
    
    // last line
    print_border_line(cols, cell_width, HORIZ_BORDER, BL_CORNER, TH_BORDER, BR_CORNER);
    cout << RESET_ALL CURSOR_SAVE;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int center_x = (cell_width - 1) / 2;
            int center_y = (cell_height - 1) / 2;
            cursor_to_cell(board, cell_width, cell_height, row, col);
            cout << cursor_right(center_x) << cursor_down(center_y);
            
            int num = board.cell_at(row, col).get_value();
            if (num != 0) {
                cout << num;
            }
        }
    }

    cout << CURSOR_RESTORE;
}
