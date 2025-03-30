#include "terminal/terminal_io.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/graphics.h"
#include <iostream>
#include <limits>
#include <string>
#include <termios.h>
#include <unistd.h>

using std::cout;
using std::cin;
using std::string;
using std::string_view;

// =-=-=-=-=-=-=-= Construcor =-=-=-=-=-=-=-= 
Terminal::Terminal() {
    cell_width = 3;
    cell_height = 1;
    p_board = nullptr;
}

// =-=-=-=-=-=-=-= Private methods =-=-=-=-=-=-=-= 
void Terminal::disable_canon_mode() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void Terminal::enable_canon_mode() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag |= ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

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
    int row,
    int col
) {
    if (row < 0) {
        row = 0;
    }
    if (col < 0) {
        col = 0;
    }
    row = std::min(row, p_board->get_rows());
    col = std::min(col, p_board->get_cols());

    // +1 for the border line
    int line = 1 * BORDER_CHAR_LEN + row * (cell_height + 1);
    int column = 1 * BORDER_CHAR_LEN + col * (cell_width + 1);
    cout << cursor_to(line, column);
}

void Terminal::paint_cell(
    string_view bg_color,
    int row,
    int col
) {
    cout << CURSOR_SAVE;
    cursor_to_cell(row, col);
    Cell cell = p_board->cell_at(row, col);
    string space(cell_width, ' ');
    string cell_value = space;
 
    // Display the number if cell is not empty 
    if (cell.get_value() != 0) {
        string value = std::to_string(cell.get_value()); 
        int cell_center = (cell_width - 1) / 2;
        cell_value.replace(cell_center, value.length(), value);
    }

    for (int row = 0; row < cell_height; row++) {
        cout << bg_color << BOLD;
        if (row == (cell_height - 1) / 2) {
            cout << cell_value;
        } else {
            cout << space;
        }

        cout << RESET_ALL << cursor_left(cell_width) << cursor_down(1);
    }

    cout << CURSOR_RESTORE; 
}

// =-=-=-=-=-=-=-= Public methods =-=-=-=-=-=-=-= 
void Terminal::set_board(Board *p_board) {
    this->p_board = p_board;
}

Board *Terminal::read_board() {
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
    Board *p_board = new Board(board_size, board_size);
    this->p_board = p_board;

    return p_board;
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

void Terminal::render_board() {
    if (!p_board) return;

    const int rows = p_board->get_rows();
    const int cols = p_board->get_cols();
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
            cursor_to_cell(row, col);
            cout << cursor_right(center_x) << cursor_down(center_y);
            
            int num = p_board->cell_at(row, col).get_value();
            if (num != 0) {
                cout << num;
            }
        }
    }

    cout << CURSOR_RESTORE;
}

void Terminal::fill_fixed_cells() {
    string finish_msg = FONT_ORANGE "[ Finish ]" RESET_ALL;
    cout << CURSOR_SAVE FONT_ORANGE << finish_msg << std::endl;
    cout << "Use the arrow keys to navigate between the cells and enter some fixed numbers (0 - 99).\n";
    cout << CURSOR_INVISIBLE;

    // current cursor position
    // cursor_to_cell();
  
    int current_row = 0;
    int current_col = 0;

    while (true) {
        
    }
}

