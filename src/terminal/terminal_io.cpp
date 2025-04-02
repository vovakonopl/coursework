#include "terminal/terminal_io.h"
#include "terminal/arrow_keys.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/graphics.h"
#include <iostream>
#include <limits>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <cctype>

#define MAX_CELL_VAL 99

using std::cout;
using std::cin;
using std::endl;
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

void Terminal::disable_echo() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);

}
void Terminal::enable_echo() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag |= ECHO;

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
        
        case 'C': 
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

void Terminal::cursor_to_cell_start( int row, int col) {
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

void Terminal::draw_cell(string_view bg_color, int row, int col) {
    cout << CURSOR_SAVE;
    cursor_to_cell_start(row, col);
    Cell cell = p_board->cell_at(row, col);
    string space(cell_width, ' ');
    string cell_value = space;
 
    // Display the number if cell is not empty 
    if (cell.get_value() != 0) {
        string value = std::to_string(cell.get_value()); 
        int cell_center = (cell_width - 1) / 2;
        cell_value.replace(cell_center, value.length(), value);
    }

    cout << bg_color;
    for (int row = 0; row < cell_height; row++) {
        if (row == (cell_height - 1) / 2) {
            if (cell.get_is_fixed()) {
                cout << BOLD;
            }

            cout << cell_value;
        } else {
            cout << space;
        }

        cout << cursor_left(cell_width) << cursor_down(1);
    }

    cout << RESET_ALL CURSOR_RESTORE; 
}

void Terminal::focus_cell(int row, int col) {
    cursor_to_cell_start(row, col);
    draw_cell(BG_LIGHT_GRAY, row, col);

    int center_x = (cell_width - 1) / 2;
    int center_y = (cell_height - 1) / 2;
    cursor_right(center_x);
    cursor_down(center_y);
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
    cout << last_char << endl;
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
            draw_cell("", row, col);
        }
    }

    cout << CURSOR_RESTORE;
}

void Terminal::fill_fixed_cells() {
    render_board();
    string finish_btn ="[ Finish ]";
    cout << FONT_ORANGE << finish_btn << RESET_ALL << endl;
    cout << "Use the arrow keys to navigate between the cells and enter some fixed numbers "
        << "(0 - " << MAX_CELL_VAL << ").\n";
    cout << CURSOR_INVISIBLE;
  
    const int rows = p_board->get_rows();
    const int cols = p_board->get_cols();
    const int board_height = 1 + rows * (cell_height + 1); // +1 for boarder lines

    focus_cell(0, 0);
    int curr_row = 0;
    int curr_col = 0;
 
    // listen each button + disable showing typed chars
    disable_canon_mode();
    disable_echo();

    bool is_btn_in_focus = false;
    while (true) {
        int new_row = curr_row;
        int new_col = curr_col;
        bool is_moved = true;

        ArrowKey arrow = read_arrow_key();
        switch (arrow) {
            case ArrowKey::Up:
                new_row--;
                break;

            case ArrowKey::Down:
                new_row++;
                break;

            case ArrowKey::Left:
                new_col--;
                break;

            case ArrowKey::Right:
                new_col++;
                break;

            case ArrowKey::None:
                is_moved = false;
                break;
        }

        if (is_btn_in_focus) {
            // go back to board
            if (arrow == ArrowKey::Up) {
                // blur button
                cout << cursor_to(board_height + 1, 0);
                cout << FONT_ORANGE << finish_btn << RESET_ALL << endl;
                
                focus_cell(curr_row, curr_col);
                is_btn_in_focus = false;
                
                continue;
            }

            if (arrow != ArrowKey::None) continue;

            char ch = cin.get();
            if (ch != '\n') continue;
            
            // finish filling board
            break;
        }

        if (is_moved) {
            if (
                new_col < 0 || new_col >= cols ||
                new_row < 0 || new_row > rows // rows included for the button under the board
            ) continue;

            // blur the current cell
            draw_cell("", curr_row, curr_col);
            
            // button focused
            if (new_row == rows) {
                is_btn_in_focus = true;
                cout << cursor_to(board_height + 1, 0);
                cout << BG_ORANGE BOLD << finish_btn << RESET_ALL << endl;
                
                continue;
            }

            curr_col = new_col;
            curr_row = new_row;
            focus_cell(curr_row, curr_col);
            
            continue;
        }

        // handle key press (add value or ignore if it isn't digit)
        Cell &cell = p_board->cell_at(curr_row, curr_col);
        char ch = cin.get();

        // ==============

        // if backspace or delete char
        if (ch == '\b' || ch == 127) {
            int value = cell.get_value() / 10;
            bool is_fixed = value; // 0 == false
            cell = Cell(value, is_fixed);

            focus_cell(curr_row, curr_col);
            continue;
        }

        if (!std::isdigit(ch)) continue;

        string value_str = std::to_string(cell.get_value());
        // set max value if there are already 2 digits
        if (value_str.length() >= 2) {
            cell = Cell(MAX_CELL_VAL, true);   
        } else {
            // ch - '0' returns this digit in int representation
            int digit = (ch - '0');
            int value = cell.get_value() * 10 + digit;
            bool is_fixed = value;
            cell = Cell(value, is_fixed);
        }

        focus_cell(curr_row, curr_col);
    }

    // restore terminal settings
    enable_canon_mode();
    enable_echo();
    cout << cursor_to(board_height + 1, 0) << RESET_ALL ERASE_CURSOR_TO_SCREEN_END CURSOR_VISIBLE;
}
