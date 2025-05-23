#include "terminal/terminal_io.h"
#include "solver/utils.h"
#include "terminal/terminal_utils.h"
#include "terminal/arrow_keys.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/graphics.h"
#include "terminal/board_sizes.h"
#include "board/board.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cctype>
#include <unistd.h>

#define MAX_CELL_VAL 99
#define DRAW_INTERVAL 300 // in ms

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
void Terminal::cursor_to_cell_start(int row, int col) {
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

    cout << BOLD FONT_BLACK;
    if (bg_color[0] == '\0') { // empty string, ""
        int color_idx = find_idx(p_board->values_on_board, cell.get_value()) % BG_COLOR_COUNT;
        if (color_idx == -1) {
            cout << bg_color;
        } else {
            cout << BG_COLORS[color_idx];
        }
    } else {
        cout << bg_color;
    }

    for (int row = 0; row < cell_height; row++) {
        if (row == (cell_height - 1) / 2) {
            if (cell.get_is_fixed()) {
                cout << FONT_WHITE;
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

bool Terminal::resize_cells() {
    if (!p_board) return false;

    int width_temp =  cell_width;   
    int height_temp = cell_height;
    
    winsize win_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
    
    if (fits_on_screen(BoardSize::Large, p_board, win_size)) {
        set_sizes(BoardSize::Large, cell_width, cell_height);
    } else if (fits_on_screen(BoardSize::Medium, p_board, win_size)) {
        set_sizes(BoardSize::Medium, cell_width, cell_height);
    } else {
        set_sizes(BoardSize::Small, cell_width, cell_height);
    }

    board_width = 1 + p_board->get_cols() * (cell_width + 1); // +1 for boarder lines
    board_height = 1 + p_board->get_rows() * (cell_height + 1);

    return width_temp != cell_width || height_temp != cell_height;
} 

// =-=-=-=-=-=-=-= Public methods =-=-=-=-=-=-=-= 
void Terminal::set_board(Board *p_board) {
    this->p_board = p_board;
    resize_cells();
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
    set_board(p_board);

    return p_board;
}

void Terminal::render_board(bool with_interval) {
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

    if (with_interval) {
        for (long unsigned int i = 0; i < p_board->result.size(); i++) {
            // draw cells with interval
            Coord coord = p_board->result.at(i);
            draw_cell("", coord.row, coord.col);

            // we need to output cells immediately to see the interval effect
            cout.flush(); 
            std::this_thread::sleep_for(std::chrono::milliseconds(DRAW_INTERVAL));
        }
    } else {
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                draw_cell("", row, col);
            }
        }
    }

    cout << CURSOR_RESTORE;
}

void Terminal::fill_fixed_cells(const char *msg) {
    render_board();

    if (msg) {
        // under the finish button and hint
        cout << CURSOR_SAVE << cursor_to(board_height + 5, 0)
            << msg << CURSOR_RESTORE;
    }
 
    string hint = 
        string("Use the arrow keys to navigate between the cells and enter some fixed numbers ") +
        "(0 - " + std::to_string(MAX_CELL_VAL) + ").\n";

    display_finish_button(board_width);
    cout << hint;
    cout << CURSOR_INVISIBLE;
  
    const int rows = p_board->get_rows();
    const int cols = p_board->get_cols();

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
        bool is_esc_sequence = false;

        // rerender board if it was resized
        bool is_resized = resize_cells(); 
        if (is_resized) {
            render_board();
         
            cout << cursor_to(board_height + 1, 0);
            display_finish_button(board_width, is_btn_in_focus);
            cout << hint;
         
            // refocus current cell
            if (!is_btn_in_focus) {
                focus_cell(curr_row, curr_col);
            }
        }

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
                is_esc_sequence = ignore_esc_sequence(); 
                break;
        }

        // detected invalid sequence (E.g. Delete key = ESC[3~)
        if (is_esc_sequence) {
            continue;
        }

        if (is_btn_in_focus) {
            // go back to board
            if (arrow == ArrowKey::Up) {
                // blur button
                cout << cursor_to(board_height + 1, 0);
                display_finish_button(board_width);
                
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
                display_finish_button(board_width, true);
                
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

        // if backspace or delete char
        if (ch == '\b' || ch == 127) {
            int value = cell.get_value() / 10;
            bool is_fixed = value; // 0 == false
            p_board->create_cell(curr_row, curr_col, value, is_fixed);

            focus_cell(curr_row, curr_col);
            continue;
        }

        if (!std::isdigit(ch)) continue;

        string value_str = std::to_string(cell.get_value());
        // set max value if there are already 2 digits
        if (value_str.length() >= 2) {
            p_board->create_cell(curr_row, curr_col, MAX_CELL_VAL, true);
        } else {
            // ch - '0' returns this digit in int representation
            int digit = (ch - '0');
            int value = cell.get_value() * 10 + digit;
            bool is_fixed = value;
            p_board->create_cell(curr_row, curr_col, value, is_fixed);
        }

        focus_cell(curr_row, curr_col);
    }

    // restore terminal settings
    enable_canon_mode();
    enable_echo();
    cout << cursor_to(board_height + 1, 0) << RESET_ALL ERASE_CURSOR_TO_SCREEN_END CURSOR_VISIBLE;
}

int Terminal::get_size() {
    cout << CURSOR_SAVE;

    int input = -1;
    while (input <= 0 || input > BOARD_MAX_SIZE) {
        cin >> input;
        if (cin.fail() || input <= 0 || input > BOARD_MAX_SIZE) {
            cin.clear();
            cout << CURSOR_RESTORE CURSOR_NEXT_LINE FONT_RED BOLD 
                << "Invalid number! Number must be in range 1 - " << BOARD_MAX_SIZE
                << RESET_ALL CURSOR_RESTORE ERASE_CURSOR_TO_LINE_END;
        }

        clear_input();
    }

    cout << ERASE_CURSOR_TO_SCREEN_END;
    return input;
}

void Terminal::ask_board_sizes(int &rows, int &cols) {
    clear_terminal();
    cout << "Creating board (max size is "
        << BOARD_MAX_SIZE << "x" << BOARD_MAX_SIZE << ")\n";

    cout << "Enter number of rows: ";
    rows = get_size();

    cout << "Enter number of cols: ";
    cols = get_size();
}

SolveMode Terminal::select_mode_menu() {
    // draw menu
    cout << ERASE_SCREEN;
    draw_menu(0);

    // listen each key press + disable showing typed chars
    disable_canon_mode();
    disable_echo();
    cout << CURSOR_INVISIBLE;

    SolveMode mode;
    bool is_selected = false;
    int focused_option = 0;

    while (!is_selected) {
        ArrowKey arrow = read_arrow_key();
        switch (arrow) {
            case ArrowKey::Up:
                focused_option--;
                if (focused_option < 0) {
                    focused_option = 2;
                }

                break;

            case ArrowKey::Down:
                focused_option++;
                if (focused_option > 2) {
                    focused_option = 0;
                }

                break;

            default:
                // ignore keys other than up/down arrows and enter
                if(ignore_esc_sequence()) continue;
                char ch = cin.get();
                if (ch != '\n') continue;
                
                // finish
                is_selected = true;
                switch (focused_option) {
                    case 0:
                        mode = SolveMode::Manual;
                        break;

                    case 1: 
                        mode = SolveMode::Computer; 
                        break;

                    default: 
                        // leave menu after exiting the program
                        draw_menu(-1); // remove focused options
                        cout << cursor_lines_down(3); // jump under the menu
                        continue;
                }
        }

        // focuse new option and blure previous
        draw_menu(focused_option);
    }

    // restore terminal settings
    enable_canon_mode();
    enable_echo();
    cout << RESET_ALL CURSOR_VISIBLE;

    if (focused_option == 2) {
        exit(0); // end program
    }

    return mode;
}
