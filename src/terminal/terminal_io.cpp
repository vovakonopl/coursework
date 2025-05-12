#include "terminal/terminal_io.h"
#include "board/board.h"
#include "solver/utils.h"
#include "terminal/arrow_keys.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/graphics.h"
#include "terminal/board_sizes.h"
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
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
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

bool Terminal::ignore_esc_sequence() {
    char ch = cin.get();
    if (ch != ESC_CHAR) {
        cin.putback(ch);
        return false;
    }

    // Double check - most sequences have [ after ESC
    ch = cin.get();
    if (ch != '[') {
        cin.putback(ch);
        cin.putback(ESC_CHAR);
        return false;
    }

    // All escape sequences ends with char between @ and ~ in ASCII table
    do {
        ch = cin.get();
    } while (ch < '@' || ch > '~');
    
    return true;
}

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

// !* helper function *!
bool fits_on_screen(BoardSize size, Board *p_board, const winsize &win_size) {
    int cell_width; 
    int cell_height;
    set_sizes(size, cell_width, cell_height);
    
    // + 1 for boarders
    int board_width = 1 + (cell_width + 1) * p_board->get_cols();  
    int board_height = 1 + (cell_height + 1) * p_board->get_rows();  
  
    // cout << CURSOR_SAVE << cursor_to(40, 20) << "board: w = " << board_width << "; h = " << board_height;
    // cout  << cursor_to(41, 20) << "window: cols = " << win_size.ws_col << "; rows = " << win_size.ws_row << CURSOR_RESTORE;

    return 
        board_width <= win_size.ws_col &&
        board_height + 4 <= win_size.ws_row; // reserve rows under the board
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

// !* helper function *!
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

// !* helper function *!
void display_finish_button(int board_width, bool is_active = false) {
    string finish_btn = "[ Finish ]";
    // left margin to center the button 
    int btn_left_margin = (board_width - finish_btn.length()) / 2; 
 
    cout << cursor_right(btn_left_margin); 
    cout << (is_active ? BG_ORANGE BOLD : FONT_ORANGE);
    cout << finish_btn << RESET_ALL << endl << endl;
}

void Terminal::fill_fixed_cells() {
    render_board();
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
            cout << CURSOR_RESTORE CURSOR_NEXT_LINE FONT_RED 
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

// !* helper function *!
string focuse_option(bool is_focused, string str) {
    string option;
    if (is_focused) {
        option += BG_ORANGE FONT_WHITE BOLD;
    }

    option += str;
    option += RESET_ALL FONT_ORANGE;

    return option;
}

// !* helper function *!
void draw_menu(int focused_opt) {
    string title("Select solving mode");
    string manual_opt("Manual solving");    
    string computer_opt("Computer solving");    
    string exit_opt("Exit program");
    
    int max_len = title.length();
    int padding_x = 4;
    int padding_y = 1;
    int option_gap = 1;

    // +2 for borders
    int box_width = max_len + padding_x * 2 + 2; 
    int box_height = option_gap * 3 + padding_y * 2 + 2 + 4; // + 4 for every line of text
        
    string horiz_border_line;
    for (int i = 0; i < box_width - 2; i++) { // save 2 chars for corners
        horiz_border_line += HORIZ_BORDER;
    }

    // draw box
    cout << CURSOR_HOME TL_CORNER << horiz_border_line << TR_CORNER CURSOR_NEXT_LINE;
    for (int i = 0; i < box_height - 2; i++) {
        cout << VERT_BORDER << cursor_right(box_width - 2) << VERT_BORDER CURSOR_NEXT_LINE;
    }
    cout << BL_CORNER << horiz_border_line << BR_CORNER;

    // write title and options
    cout << FONT_ORANGE;
    int pos_y = padding_y + 2; // +2 - after top border
    int pos_x = padding_x + 2; // +2 - after left border
    cout << cursor_to(pos_y, pos_x) << title;
 
    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "1) "
        << focuse_option(focused_opt == 0, manual_opt);

    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "2) "
        << focuse_option(focused_opt == 1, computer_opt);

    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "3) "
        << focuse_option(focused_opt == 2, exit_opt);

    cout << RESET_ALL;
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
